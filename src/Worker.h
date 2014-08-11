#ifndef __WORKER_H__
#define __WORKER_H__

#include <memory>
#include <thread>

#include "Poll.h"
#include "Socket.h"

class Worker
{
public:
	struct Stat
	{
		uint64_t currentConnections;
		uint64_t totalConnections;
	};
	
	Worker();
	virtual ~Worker();
	void operator()();
	void addFD(SocketPtr sock, Poll::PollHandler handler);
	void start();
	size_t size() const;
	const Stat& stat();

private:
	Poll poll_;
	std::shared_ptr<std::thread> thread_;
	Stat stat_;
};


#endif /* __WORKER_H__ */
