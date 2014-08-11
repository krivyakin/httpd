#ifndef __ABOUT_REQUEST_PROCESSOR_H__
#define __ABOUT_REQUEST_PROCESSOR_H__

#include "RequestProcessor.h"
#include "Server.h"

class AboutRequestProcessor : public RequestProcessor
{
public:
	AboutRequestProcessor(ServerPtr server);

	int process(HttpRequest& req, SocketPtr sock) override;
	bool isEligible(const HttpRequest& req) const override;

private:
	ServerPtr server_;
};

#endif /* __ABOUT_REQUEST_PROCESSOR_H__ */
