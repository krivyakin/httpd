#ifndef __ERROR_REQUEST_PROCESSOR__
#define __ERROR_REQUEST_PROCESSOR__

#include "RequestProcessor.h"

class ErrorRequestProcessor : public RequestProcessor
{
public:
	ErrorRequestProcessor();

	void setLastErrorCode(int code);
	int process(HttpRequest& req, SocketPtr sock) override;
	bool isEligible(const HttpRequest& req) const override;

private:
	int code_;
};

#endif
