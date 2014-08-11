#include "Worker.h"

Worker::Worker()
: poll_(100000)
{

}

Worker::~Worker()
{

}

void Worker::operator()()
{
	poll_.dispatch();
}
	
void Worker::addFD(SocketPtr sock, Poll::PollHandler handler)
{
	poll_.addFD(sock, handler);
	++stat_.totalConnections;
}

void Worker::start()
{
	thread_ = std::make_shared<std::thread>(
		std::bind(&Worker::operator(), this));
}

size_t Worker::size() const
{
	return poll_.size();
}

const Worker::Stat& Worker::stat()
{
	stat_.currentConnections = poll_.size();
	return stat_;
}
