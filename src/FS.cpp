#include <memory>

#include "FS.h"
#include "Log.h"

namespace FS
{

File::File(const std::string path, const std::string name)
: path_(path)
, name_(name)
{
	fd_ = open(path.c_str(), O_RDONLY);
	if (fd_ == -1) {
		fd_ = -errno;
		int ret = stat(path.c_str(), &stat_);
		if (ret == -1)
		{
			THROW_SYSTEM_ERROR();
		}
	} else {
		int ret = fstat(fd_, &stat_);
		if (ret == -1)
		{
			THROW_SYSTEM_ERROR();
		}
	}
}

File::~File()
{

}

bool File::isRegular() const
{
	return S_ISREG(stat_.st_mode);
}

bool File::isDir() const
{
	return S_ISDIR(stat_.st_mode);
}

size_t File::size() const
{
	return stat_.st_size;
}
	
const std::string& File::path() const
{
	return path_;
}
	
const std::string& File::name() const
{
	return name_;
}

Directory::Directory(const std::string path)
: path_(path)
{
	dir_ = opendir(path.c_str());
	if (!dir_)
	{
		THROW_SYSTEM_ERROR();
	}
}

Directory::~Directory()
{
	if (dir_)
	{
		closedir(dir_);
	}
}

FilePtr Directory::nextFile()
{
	struct dirent *epdf;

	errno = 0;
	epdf = readdir(dir_);
	if (epdf == NULL)
	{
		if (errno) {
			THROW_SYSTEM_ERROR();
		}
		return FilePtr();
	}

	FilePtr file = std::make_shared<File>(
		std::string(path_ + epdf->d_name).c_str(), epdf->d_name);

	return file;
}

}
