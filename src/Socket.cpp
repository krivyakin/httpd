#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Log.h"
#include "Socket.h"

Address::Address(std::string _host, uint16_t _port)
: host(_host)
, port(_port)
{

}

Socket::Socket(Type type)
: type_(type)
, needToWrite_(false)
{
	fd_ = socket(typeToCType(type_), SOCK_STREAM, 0);
	if (fd_ == -1)
	{
		THROW_SYSTEM_ERROR();
	}
	int one = 1;
	int ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
	if (ret != 0)
	{
		THROW_SYSTEM_ERROR();
	}
}

Socket::Socket(Socket&& rhs)
: fd_(std::move(rhs.fd_))
, addr_(rhs.addr_)
, type_(rhs.type_)
, host_(rhs.host_)
, port_(rhs.port_)
{
	rhs.fd_ = 0;
}

Socket::~Socket()
{

}

void Socket::bind(Address address)
{
	struct hostent *host = gethostbyname(address.host.c_str());

	if (!host)
	{
		const char *errMsg = hstrerror(h_errno);
		_E(errMsg);
		throw std::runtime_error(errMsg);
	}

	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = typeToCType(type_);
	addr_.sin_port = htons(address.port);
	//TODO: use correct host
	//memcpy(&addr_.sin_addr.s_addr, host->h_addr, host->h_length);
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = ::bind(fd_, reinterpret_cast<struct sockaddr*>(&addr_),
		sizeof(addr_));
	if (ret != 0)
	{
		THROW_SYSTEM_ERROR();
	}
}

void Socket::listen()
{
	int ret = ::listen(fd_, 10000);

	if (ret != 0)
	{
		THROW_SYSTEM_ERROR();
	}
}

SocketPtr Socket::accept()
{
	socklen_t size = sizeof(addr_);
	int ret = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&addr_), &size);

	if (ret == -1)
	{
		THROW_SYSTEM_ERROR();
	}

	return std::make_shared<Socket>(ret);
}

void Socket::makeNonBlocking()
{
	long one = 1;

	int ret = ioctl(fd_, FIONBIO, &one);

	if (ret == -1)
	{
		THROW_SYSTEM_ERROR();
	}
}

void Socket::read(Buffer<uint8_t>& buffer)
{
	uint8_t buf[1024];
	int ret = 0;

	ret = recv(fd_, buf, sizeof(buf), 0);
	while (ret > 0)
	{
		buffer.append(buf, ret);
		ret = recv(fd_, buf, sizeof(buf), 0);
	}
}

size_t Socket::write(const uint8_t *buf, size_t size)
{
	writeBuffer_.append(buf, size);

	return write();
}

size_t Socket::write()
{
	int ret = 0;

	ret = ::write(fd_, writeBuffer_.data(), writeBuffer_.size());
	if (ret == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			_E("Can't write to socket. Socket will be closed");
		}
	} else {
		writeBuffer_.drain(ret);
	}

	needToWrite_ = !!writeBuffer_.size();

	return writeBuffer_.size();
}

int Socket::native()
{
	return fd_;
}

bool Socket::needToWrite() const
{
	return needToWrite_;
}

Socket::Socket(int fd)
: fd_(fd)
{

}

int Socket::typeToCType(Type type) const
{
	switch (type)
	{
		case Type::UNIX:
			return AF_UNIX;

		case Type::INET:
			return AF_INET;
	}

	return -1;
}

bool Socket::error()
{
	int optval;
	socklen_t optlen;

	optlen = sizeof(optval);
	getsockopt(fd_, SOL_SOCKET, SO_ERROR, &optval, (socklen_t *)&optlen);

	return optval;
}

bool operator==(SocketPtr lhs, int rhs)
{
	return lhs->fd_ == rhs;
}
