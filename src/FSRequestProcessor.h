#ifndef __FS_REQUEST_PROCESSOR_H__
#define __FS_REQUEST_PROCESSOR_H__

#include "Buffer.h"
#include "FS.h"
#include "RequestProcessor.h"

class FSRequestProcessor : public RequestProcessor
{
public:
	FSRequestProcessor();

	int process(HttpRequest& req, SocketPtr sock) override;
	bool isEligible(const HttpRequest& req) const override;

private:
	void fileContent(HttpRequest& req, FS::File& file, Buffer<uint8_t>& buffer);
	void dirContent(HttpRequest& req, FS::File& file, Buffer<uint8_t>& buffer);

};

#endif /* __FS_REQUEST_PROCESSOR_H__ */
