#ifndef __SERVER_H__
#define __SERVER_H__

#include <vector>
#include <memory>

#include "RequestProcessor.h"
#include "Socket.h"
#include "Worker.h"

class Server : public std::enable_shared_from_this<Server>
{
public:
	Server(uint16_t port, size_t threads);
	virtual ~Server();
	void start();

	int handler(SocketPtr sock, Poll::EventType eventType);
	void stat(std::vector<Worker::Stat>& stats);
	void addReqProcessor(RequestProcessorPtr processor);

protected:
	size_t chooseWorker();

	Socket listener_;
	uint16_t port_;
	std::vector<Worker> workers_;
	std::vector<RequestProcessorPtr> processors_;
};

typedef std::shared_ptr<Server> ServerPtr;

#endif /* __SERVER_H__ */
