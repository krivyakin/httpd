#ifndef __LOG_H__
#define __LOG_H__

#include <string.h>

#include <cerrno>
#include <iostream>
#include <system_error>

#define __STR(s) #s
#define WHERE __STR(__FILE__ ":" __LINE__)

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define _E(msg) \
	std::cerr << "[ERROR] " << __FILENAME__ << ":" << __LINE__ << " " << msg << std::endl;

#define _W(msg) \
	std::cerr << "[WARNING] " << __FILENAME__ << ":" << __LINE__ << " " << msg << std::endl;

#define _I(msg) \
	std::cerr << "[INFO] " << __FILENAME__ << ":" << __LINE__ << " " << msg << std::endl;

#define SYSTEM_ERROR() \
do { \
	_E(strerror(errno));\
} while (0)

#define THROW_SYSTEM_ERROR() \
do { \
	SYSTEM_ERROR(); \
	throw std::system_error(std::error_code(errno, std::system_category()));\
} while (0)

#define SYSTEM_WARNING() \
do { \
	_W(strerror(errno)) \
} while (0)

#define THROW(msg) \
do { \
	throw std::runtime_error(msg);\
} while (0)

#define THROW_LOGIC(msg) \
do { \
	_E(msg);\
	throw std::logic_error(msg);\
} while (0)

#endif /* __LOG_H__ */
