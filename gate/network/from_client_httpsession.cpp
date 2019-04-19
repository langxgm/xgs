
#include "from_client_httpsession.h"

#include <event2/http.h>
#include <evpp/http/context.h>

#include <sstream>

From_Client_HttpSession::From_Client_HttpSession()
{

}

From_Client_HttpSession::~From_Client_HttpSession()
{

}

void From_Client_HttpSession::RegisterCallback()
{
#ifdef _DEBUG
	REG_DCALLBACK(&From_Client_HttpSession::HandleDefault, this);
#endif
}

void From_Client_HttpSession::HandleDefault(evpp::EventLoop* loop, const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& respcb)
{
	std::ostringstream oss;
#ifdef _DEBUG
	oss << " ip=" << ctx->remote_ip() << "\n"
#else
	oss << " ip=" << evpp::http::Context::FindClientIPFromURI(ctx->uri().c_str(), ctx->uri().length()) << "\n"
#endif
		<< " uri=" << ctx->uri() << "\n"
		<< " body=" << ctx->body().ToString() << "\n";

	ctx->set_response_http_code(HTTP_NOTFOUND);
	respcb(oss.str());
}
