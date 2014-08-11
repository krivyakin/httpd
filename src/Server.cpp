#include <limits>

#include "AboutRequestProcessor.h"
#include "ErrorRequestProcessor.h"
#include "HttpRequest.h"
#include "Server.h"

Server::Server(uint16_t port, size_t threads)
: listener_(Socket::Type::INET)
, port_(port)
, workers_(threads)
{
	//TODO: host should not be hardcoded
	listener_.bind(Address("0.0.0.0", port));
	listener_.listen();

	for (size_t i = 0; i  < workers_.size(); ++i)
	{
		workers_[i].start();
	}
}

Server::~Server()
{

}

size_t Server::chooseWorker()
{
	size_t idx = 0;
	size_t count = std::numeric_limits<int>::max();

	for (size_t i = 0; i < workers_.size(); ++i)
	{
		if (workers_[i].size() < count)
		{
			idx = i;
			count = workers_[i].size();
		}
	}

	return idx;
}

void Server::start()
{
	_I("Server started at 0.0.0.0:" << port_);
	_I("You can find connections statistic at http://127.0.0.1:" << port_ << "/about");
	while (1)
	{
		SocketPtr s = listener_.accept();
		s->makeNonBlocking();

		size_t idx = chooseWorker();
		workers_[idx].addFD(s, std::bind(&Server::handler, this,
			std::placeholders::_1, std::placeholders::_2));
	}
}

void Server::stat(std::vector<Worker::Stat>& stats)
{
	for (auto it = workers_.begin(); it != workers_.end(); ++it)
	{
		stats.push_back(it->stat());
	}
}

int Server::handler(SocketPtr sock, Poll::EventType eventType)
{
	if (eventType == Poll::EventType::IN)
	{
		Buffer<uint8_t> buf;

		sock->read(buf);
		std::string requestStr(
			reinterpret_cast<const char*>(buf.data()), buf.size());

		HttpRequest request(requestStr);

		if (buf.size() == 0)
			return -1;

		for (auto it = processors_.begin(); it != processors_.end(); ++it)
		{
			if ((*it)->isEligible(request))
			{
				//TODO: Exceptions may be too expensive. Replace it by return codes
				try {
					return (*it)->process(request, sock);
				} catch (std::system_error& e) {
					/*
					 * TODO: it's very very bad to create processor every time.
					 * Every worker should have own ErrorRequestProcessor
					 */
					ErrorRequestProcessor ep;
					ep.setLastErrorCode(e.code().value());
					ep.process(request, sock);

					_E(e.what());
					return -1;
				}
			}
		}

		return -1;
	}
	else if (eventType == Poll::EventType::OUT)
	{
		if (sock->needToWrite() && sock->write() == 0)
			return -1;
	}

	return 0;
}

void Server::addReqProcessor(RequestProcessorPtr processor)
{
	processors_.push_back(processor);
}
