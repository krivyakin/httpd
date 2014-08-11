#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include <string>
#include <vector>

class HttpRequest
{
public:
	enum class Version
	{
		v1_0,
		v1_1
	};

	enum class Method
	{
		GET,
	};

	HttpRequest(const std::string& req);
	std::string versionToStr() const;

	Version version;
	Method method;
	std::string path;

	std::map<std::string, std::string> hdr;

private:
	int parse(const std::string& req);
};

#endif /* __HTTP_REQUEST_H__ */
