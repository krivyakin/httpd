#include <iostream>

#include "HttpRequest.h"
#include "Log.h"

HttpRequest::HttpRequest(const std::string& req)
{
	hdr["Host"] = "";
	hdr["User-Agent"] = "";
	hdr["Connection"] = "";

	parse(req);
}

std::string HttpRequest::versionToStr() const
{
	if (version == Version::v1_1) {
		return "1.1";
	}

	//TODO: what about 0.9?
	return "1.0";
}

/* TODO: it's very ugly parsing. Need to optimize it for more methods/header/...*/
int HttpRequest::parse(const std::string& req)
{
	size_t pos = 0;

	if (req.substr(pos, 3) == "GET") {
		method = Method::GET;
		pos += 4;
	} else {
		return -1;
	}

	size_t end = req.find(" ", 4);
	path = req.substr(pos, end - pos);
	pos = end + 1;

	if (req.substr(pos, 5) != "HTTP/")
		return -2;
	pos += 5;

	std::string ver = req.substr(pos, 3);
	if (ver == "1.0") {
		version = Version::v1_0;
	} else if (ver == "1.1") {
		version = Version::v1_1;
	} else {
		return -3;
	}

	for (auto it = hdr.begin(); it != hdr.end(); ++it)
	{
		const std::string& hdr = it->first;
		size_t pos = req.find(hdr);

		pos += hdr.size() + 2;
		size_t end = req.find("\n", pos);
		std::string value = req.substr(pos, end - pos - 1);
		if (value.back() == '\r')
		{
			value.erase(value.size() - 1, 1);
		}

		it->second = value;
	}

	_I("Request " << path);

	return 0;
}
