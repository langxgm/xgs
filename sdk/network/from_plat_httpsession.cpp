
#include "from_plat_httpsession.h"

#include <evpp/http/context.h>

#include <sstream>

From_Plat_HttpSession::From_Plat_HttpSession()
{

}

From_Plat_HttpSession::~From_Plat_HttpSession()
{

}

void From_Plat_HttpSession::RegisterCallback()
{
	REG_DCALLBACK(&From_Plat_HttpSession::HandleDefault, this);
}

void From_Plat_HttpSession::HandleDefault(evpp::EventLoop* loop, const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& respcb)
{
	std::ostringstream oss;
	oss << " ip=" << ctx->remote_ip() << "\n"
		<< " uri=" << ctx->uri() << "\n"
		<< " body=" << ctx->body().ToString() << "\n";
	respcb(oss.str());
}
