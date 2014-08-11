#ifndef __REQUEST_PROCESSOR_H__
#define __REQUEST_PROCESSOR_H__

#include <memory>
#include <sstream>

#include "HttpRequest.h"
#include "Socket.h"

class RequestProcessor
{
public:
	enum HttpStatus {
		OK = 200,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		INTERNAL_ERROR = 500
	};

	RequestProcessor() { };

	virtual int process(HttpRequest& req, SocketPtr sock) = 0;
	virtual bool isEligible(const HttpRequest& req) const = 0;

	std::string StatusToStr(HttpStatus status)
	{
		switch (status)
		{
		case OK:
			return "OK";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not found";
		case INTERNAL_ERROR:
			return "Internal Server Error";
		}

		return "Internal Server Error";
	}

	void makeHttpResponse(HttpRequest& req, const char *content, size_t size,
		Buffer<uint8_t>& buffer, HttpStatus status = OK)
	{
		std::stringstream ss;
		ss << "HTTP/" << req.versionToStr() << " " << status <<
			StatusToStr(status) << "\nContent-Length:" <<
			size << "\n\n";

		buffer.append(reinterpret_cast<const uint8_t*>(ss.str().data()), ss.str().size());
		buffer.append(reinterpret_cast<const uint8_t*>(content), size);
	}
};

typedef std::shared_ptr<RequestProcessor> RequestProcessorPtr;

#endif /* __REQUEST_PROCESSOR_H__ */
