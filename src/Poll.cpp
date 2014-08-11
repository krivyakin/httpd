#include <sys/epoll.h>

#include <memory>

#include "Log.h"
#include "Poll.h"

static const size_t kMaxEvents = 1 << 16;

template<typename T, typename D>
static std::unique_ptr<T, D> __memBlock(size_t size, D deleter)
{
	return std::unique_ptr<T, D>(reinterpret_cast<T*>(
		calloc(size, sizeof(T))), deleter);
}
#define UNIQ_MEM_PTR(T, size, D) __memBlock<T, decltype(&D)>(size, &D)

Poll::Poll(size_t size)
{
	fd_ = epoll_create(size);
}

Poll::~Poll()
{

}

void Poll::addFD(SocketPtr sock, PollHandler handler)
{
	static struct epoll_event ev;

	{
		std::unique_lock<std::mutex> lock(mutex_);
		handlers_[sock->native()] = std::make_tuple(sock, handler);
	}

	ev.events = /*EPOLLET |*/ EPOLLIN | EPOLLOUT;
	ev.data.fd = sock->native();

	int ret = epoll_ctl(fd_, EPOLL_CTL_ADD, sock->native(), &ev);
	if (ret != 0)
	{
		THROW_SYSTEM_ERROR();
	}
}

void Poll::dispatch()
{
	auto events = UNIQ_MEM_PTR(struct epoll_event, kMaxEvents, free);

	while (1)
	{
		int nfds = epoll_wait(fd_, events.get(), kMaxEvents, -1);

		if (nfds == -1 && errno != EINTR)
		{
			THROW_SYSTEM_ERROR();
		}

		for (int i = 0; i < nfds; ++i)
		{
			int fd = events.get()[i].data.fd;

			{
				//TODO: may be we don't need to lock the whole block
				std::unique_lock<std::mutex> lock(mutex_);

				if (handlers_.count(fd) == 0)
					continue;

				SocketPtr socket = std::get<0>(handlers_[fd]);
				PollHandler handler = std::get<1>(handlers_[fd]);

				if (events.get()[i].events & EPOLLIN)
				{
					if (handler(socket, EventType::IN) < 0)
					{
						handlers_.erase(fd);
						continue;
					}
				}
				if (events.get()[i].events & EPOLLOUT)
				{
					if (handler(socket, EventType::OUT) < 0)
					{
						handlers_.erase(fd);
						continue;
					}
				}
			}
		}
	}

}

size_t Poll::size() const
{
	return handlers_.size();
}
