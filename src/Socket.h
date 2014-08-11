#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <arpa/inet.h>

#include <memory>
#include <string>

#include "Buffer.h"
#include "FileDescriptor.h"

struct Address {
	Address(std::string _host, uint16_t _port);
	std::string host;
	uint16_t port;
};

class Socket;
typedef std::shared_ptr<Socket> SocketPtr;

class Socket
{
public:
	enum class Type {
		UNIX,
		INET
	};

	Socket(Type type);
	Socket(int fd);
	Socket(Socket&&);
	virtual ~Socket();

	void bind(Address address);
	void listen();
	SocketPtr accept();
	void makeNonBlocking();
	void read(Buffer<uint8_t>& buffer);
	/* return writeBuffer_ size */
	size_t write(const uint8_t *buf, size_t size);
	size_t write();
	int native();
	bool error();
	bool needToWrite() const;

	friend bool operator==(SocketPtr lhs, int rhs);

private:
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	int typeToCType(Type type) const;
	FileDescriptor fd_;
	struct sockaddr_in addr_;

	Type type_;
	std::string host_;
	uint16_t port_;
	Buffer<uint8_t> writeBuffer_;
	bool needToWrite_;
};


#endif /* __SOCKET_H__ */
