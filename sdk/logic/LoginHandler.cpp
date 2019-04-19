
#include "LoginHandler.h"
#include "sdk/network/from_ws_session.h"
#include "sdk/network/to_plat_httpsession.h"
#include "sdk/logic/HandleMsgMacros.h"

#include "sdk/config/SystemConfig.h"
#include "sdk/pbconfig/sdk.conf.pb.h"
#include "sdk/pbconfig/wxappinfo.conf.pb.h"

#include "sdk/protos/login2sdk.pb.h"

#include "xshare/work/WorkDispatcher.h"
#include "xshare/config/PBConfigManager.h"

#include <event2/http.h> // HTTP_OK
#include <evpp/httpc/request.h>
#include <evpp/httpc/response.h>
#include <libgo/coroutine.h>
#include <third_party/rapidjson/document.h>

void LoginHandler::PreInitDatabase()
{

}

void LoginHandler::ListenFromLs(ServerWorker* pServer)
{
	REG_MSG_TO(pServer, &LoginHandler::HandleL2SDKLoginWeixin, this, protos::L2SDKLoginWeixin, "登陆请求(微信平台)");
}

void LoginHandler::HandleL2SDKLoginWeixin(const MessagePtr& pMsg, int64_t nSessionID, const MessageMetaPtr& pMeta)
{
	MSG_CHECK_OF(protos::L2SDKLoginWeixin);

	size_t nHash = std::hash<std::string>()(pHandleMsg->param().deviceid());
	WorkDispatcherAsync(nHash).RunInLoop([=]() {
		go[=]() {

			// 发送登陆错误消息
			auto SendLoginError = [pHandleMsg, nSessionID, pMeta](int nError, const std::string& strErrMsg, int nWXErrCode, const std::string& strWXErrMsg)
			{
				protos::SDK2LLoginWeixin send;
				send.set_error(nError);
				send.set_errmsg(strErrMsg);
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_link_sessionid(pHandleMsg->link_sessionid());
				send.set_wx_errcode(nWXErrCode);
				send.set_wx_errmsg(strWXErrMsg);
				From_Ws_Session::Me()->Send(nSessionID, &send, *pMeta);
			};

			// 发送登陆成功消息
			auto SendLoginSuccess = [pHandleMsg, nSessionID, pMeta](std::string strOpenID, std::string strSessionKey)
			{
				protos::SDK2LLoginWeixin send;
				send.set_error(0);
				send.set_allocated_route(pHandleMsg->release_route());
				send.set_link_sessionid(pHandleMsg->link_sessionid());
				send.set_allocated_param(pHandleMsg->release_param());
				send.set_wx_openid(std::move(strOpenID));
				send.set_wx_session_key(std::move(strSessionKey));
				From_Ws_Session::Me()->Send(nSessionID, &send, *pMeta);
			};

			auto pWXAppInfo = PBConfigManager::Me()->At<std::string, const pbconfig::WXAppInfo*>("myapp");
			if (pWXAppInfo == nullptr)
			{
				std::string strErrMsg = std::string("pbconfig::WXAppInfo not found appname=") + "myapp";
				LOG(ERROR) << strErrMsg;
				SendLoginError(-10, strErrMsg, 0, "");
				return;
			}

			// URL请求参数
			// appid		string		是	小程序 appId
			// secret		string		是	小程序 appSecret
			// js_code		string		是	登录时获取的 cod
			// grant_type	string		是	填写为 authorization_code

			const std::string& strAppID = pWXAppInfo->appid();
			const std::string& strSecret = pWXAppInfo->secret();
			const std::string& strJsCode = pHandleMsg->param().js_code();
			const std::string strGrantType = "client_credential";// "authorization_code";

			std::string strUriParam;
			strUriParam.reserve(strAppID.length() + strSecret.length() + strJsCode.length() + strGrantType.length());
			strUriParam += "/sns/jscode2session";
			strUriParam += "?appid=" + strAppID;
			strUriParam += "&secret=" + strSecret;
			strUriParam += "&js_code=" + strJsCode;
			strUriParam += "&grant_type=" + strGrantType;

			// 打印HTTP请求
			LOG_FIRST_N(INFO, 10) << "HTTP HOST:" << To_Plat_HttpSession::Me()->GetHost()
				<< " PORT:" << To_Plat_HttpSession::Me()->GetPort()
				<< " TIMEOUT:" << To_Plat_HttpSession::Me()->GetTimeout()
				<< " URI_PARAM:" << strUriParam;

			// 内网测试模式
			if (SystemConfig::Me()->GetSdkConfig()->networkmodule().to_plat_http_testmode())
			{
				std::string strOpenID = "opid:" + strJsCode;
				std::string strWXSessionKey = "wxsessionkey:" + strJsCode;
				SendLoginSuccess(strOpenID, strWXSessionKey);
				// 打印登陆日志
				LOG(INFO) << "login test mode openid=" << strOpenID << " session_key=" << strWXSessionKey;
				return;
			}

			auto httpHandler = [SendLoginError, SendLoginSuccess](const std::shared_ptr<evpp::httpc::Response>& r) {

				std::unique_ptr<const evpp::httpc::Request> pReq(r->request());

				do
				{
					if (r->http_code() != HTTP_OK)
					{
						LOG(ERROR) << "HTTP-GET Result: http_code=" << r->http_code();
						SendLoginError(r->http_code(), "HTTP-GET faild", 0, ""); // HTTP访问失败,HTTP错误代码
						break;
					}

					LOG_FIRST_N(WARNING, 3) << "HTTP-GET Result: body=" << r->body().ToString();

					rapidjson::Document doc;
					if (doc.Parse(r->body().data(), r->body().size()).HasParseError())
					{
						LOG(ERROR) << "HTTP-GET Result: json parse error code=" << doc.GetParseError()
							<< " offset=" << doc.GetErrorOffset();
						SendLoginError(-20, "HTTP-GET Result: json parse error", 0, "");
						break;
					}

					auto itErrCode = doc.FindMember("errcode");
					if (itErrCode != doc.MemberEnd())
					{
						if (itErrCode->value.IsNumber() == false)
						{
							LOG(ERROR) << "HTTP-GET Result: weixin errcode type non numeric";
							SendLoginError(-30, "HTTP-GET Result: weixin errcode type non numeric", 0, ""); // 没有获得errcode的值
							break;
						}
						else
						{
							int nWXErrCode = itErrCode->value.GetInt();
							if (nWXErrCode != 0)
							{
								auto itErrMsg = doc.FindMember("errmsg");
								if (itErrMsg == doc.MemberEnd() || itErrMsg->value.IsString() == false)
								{
									LOG(ERROR) << "HTTP-GET Result: weixin errmsg is invalid"
										<< " errcode=" << nWXErrCode;
									SendLoginError(-32, "HTTP-GET Result: weixin errmsg is invalid", nWXErrCode, ""); // 没有返回errMsg
									break;
								}

								LOG(ERROR) << "HTTP-GET Result: errcode=" << nWXErrCode
									<< " errmsg=" << itErrMsg->value.GetString();
								SendLoginError(-31, "HTTP-GET Result: weixin error", nWXErrCode, itErrMsg->value.GetString()); // 没有返回errMsg
								break;
							}
						}
					}

					auto itOpenID = doc.FindMember("openid");
					if (itOpenID == doc.MemberEnd() || itOpenID->value.IsString() == false)
					{
						LOG(ERROR) << "HTTP-GET Result: openid is invalid";
						SendLoginError(-40, "HTTP-GET Result: openid is invalid", 0, ""); // 没有返回openid
						break;
					}

					auto itSessionKey = doc.FindMember("session_key");
					if (itSessionKey == doc.MemberEnd() || itSessionKey->value.IsString() == false)
					{
						LOG(ERROR) << "HTTP-GET Result: session_key is invalid";
						SendLoginError(-50, "HTTP-GET Result: session_key is invalid", 0, ""); // 没有返回session_key
						break;
					}

					// 成功
					SendLoginSuccess(itOpenID->value.GetString(), itSessionKey->value.GetString());

					// 打印登陆日志
					LOG(INFO) << "login weixin openid=" << itOpenID->value.GetString() << " session_key=" << itSessionKey->value.GetString();

				} while (false);
			};

#if (1)
			// 连接池请求
			To_Plat_HttpSession::Me()->DoGet(httpHandler, strUriParam);
#else
			// 新建请求
			std::string strUrl = SystemConfig::Me()->GetSdkConfig()->networkmodule().to_plat_http_host();
			double fTimeout = SystemConfig::Me()->GetSdkConfig()->networkmodule().to_plat_http_timeout();
			if (strUrl.find("https") != 0)
			{
				strUrl = "https://" + strUrl;
			}
			strUrl += strUriParam;
			To_Plat_HttpSession::Me()->DoGetWithUrl(httpHandler, strUrl, fTimeout);
#endif
		};
	});
}
