#include <unistd.h>

#include "FileDescriptor.h"
#include "Log.h"
#include "Socket.h"

FileDescriptor::FileDescriptor(int fd)
: fd_(fd)
{

}

FileDescriptor::FileDescriptor(FileDescriptor&& rhs)
: fd_(rhs.fd_)
{
	rhs.fd_ = -1;
}

FileDescriptor::~FileDescriptor()
{
	if (fd_ > 0 && close(fd_) != 0)
	{
		SYSTEM_WARNING();
	}
}

FileDescriptor& FileDescriptor::operator=(int fd)
{
	fd_ = fd;
	return *this;
}

FileDescriptor::operator int()
{
	return fd_;
}
