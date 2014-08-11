#ifndef __FILE_DESCRIPTOR_H__
#define __FILE_DESCRIPTOR_H__

class FileDescriptor
{
public:
	FileDescriptor(int fd = -1);
	FileDescriptor(FileDescriptor&&);
	~FileDescriptor();

	FileDescriptor& operator=(int fd);

	operator int();

private:
	FileDescriptor(const FileDescriptor&) = delete;
	FileDescriptor& operator=(const FileDescriptor&) = delete;

	int fd_;
};

#endif /* __FILE_DESCRIPTOR_H__ */
