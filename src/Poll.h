#ifndef __POLL_H__
#define __POLL_H__

#include <functional>
#include <map>
#include <mutex>
#include <tuple>

#include "FileDescriptor.h"
#include "Socket.h"

class Poll
{
public:
	enum class EventType
	{
		IN,
		OUT,
		CLOSE,
		ERROR,
	};

	typedef std::function<int(SocketPtr, EventType)> PollHandler;

	Poll(size_t size);
	virtual ~Poll();
	void addFD(SocketPtr sock, PollHandler handler);
	void dispatch();
	size_t size() const;

private:
	FileDescriptor fd_;
	std::map<int, std::tuple<SocketPtr, PollHandler> > handlers_;
	std::mutex mutex_;
};

#endif /* __POLL_H__ */
