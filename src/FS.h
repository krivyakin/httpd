#ifndef __FS_H__
#define __FS_H__

#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "Buffer.h"
#include "FileDescriptor.h"

namespace FS
{

class File
{
public:
	File(const std::string path, const std::string name);
	virtual ~File();
	bool isRegular() const;
	bool isDir() const;
	size_t size() const;
	const std::string& path() const;
	const std::string& name() const;

	template<typename T>
	void content(Buffer<T>& buffer)
	{
		if (fd_ < 0) {
			errno = -fd_;
			THROW_SYSTEM_ERROR();
		}

		if (lseek(fd_, SEEK_SET, 0) == -1)
		{
			THROW_SYSTEM_ERROR();
		}

		T tmpBuf[1024 * 10];
		int ret = read(fd_, tmpBuf, sizeof(tmpBuf));
		while (ret)
		{
			if (ret == -1)
			{
				THROW_SYSTEM_ERROR();
			}

			buffer.append(tmpBuf, ret);
			ret = read(fd_, tmpBuf, sizeof(tmpBuf));
		}
	}


private:
	std::string path_;
	std::string name_;
	struct stat stat_;
	FileDescriptor fd_;
};

typedef std::shared_ptr<File> FilePtr;

class Directory
{
public:
	Directory(const std::string path);
	~Directory();
	FilePtr nextFile();

private:
	DIR *dir_;
	std::string path_;
};

}

#endif /* __FS_H__ */
