#include <system_error>

#include "ErrorRequestProcessor.h"

ErrorRequestProcessor::ErrorRequestProcessor()
{

}

int ErrorRequestProcessor::process(HttpRequest& req, SocketPtr sock)
{
	std::string content;
	HttpStatus status;

	if (code_ == EACCES) {
		status = FORBIDDEN;
		content = "403 ";
	} else if (code_ == ENOENT) {
		status = NOT_FOUND;
		content = "404 ";
	} else {
		status = INTERNAL_ERROR;
		content = "500 ";
	}

	content += req.path;

	Buffer<uint8_t> buffer;
	makeHttpResponse(req, content.data(), content.size(), buffer, status);

	if (sock->write(buffer.data(), buffer.size()) == 0)
	{
		return -1;
	}

	(void)req;
	return 0;
}

bool ErrorRequestProcessor::isEligible(const HttpRequest& req) const
{
	(void)req;
	return true;
}

void ErrorRequestProcessor::setLastErrorCode(int code)
{
	code_ = code;
}
