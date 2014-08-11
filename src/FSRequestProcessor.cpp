#include <sstream>
#include "FS.h"
#include "FSRequestProcessor.h"

FSRequestProcessor::FSRequestProcessor()
{

}

void FSRequestProcessor::fileContent(HttpRequest& req,
	FS::File& file, Buffer<uint8_t>& buffer)
{
	Buffer<char> content;
	/*
	 * TODO: it's will be better to make cache to avoid reading every time.
	 * Posible to use inotify to get event when file should be reloaded
	 */
	file.content(content);

	makeHttpResponse(req, content.data(), content.size(), buffer);
}

void FSRequestProcessor::dirContent(HttpRequest& req,
	FS::File& file, Buffer<uint8_t>& buffer)
{
	FS::Directory dir(file.path());

	std::stringstream ss;
	FS::FilePtr f;

	ss << "<table>";

	while ((f = dir.nextFile()))
	{
		ss << "<tr><td><a href='" << f->name() <<
			(f->isDir() ? "/" : "") << "'>" << f->name() <<
			"</a></td><td>" << f->size() << "</td></tr>";
	}
	ss << "</table>";

	std::string content = ss.str();

	makeHttpResponse(req, content.data(), content.size(), buffer);
}

int FSRequestProcessor::process(HttpRequest& req, SocketPtr sock)
{
	const std::string path = std::string(".") + req.path;
	FS::File file(path, path);

	Buffer<uint8_t> buffer;

	if (file.isRegular()) {
		fileContent(req, file, buffer);
	} else if (file.isDir()) {
		dirContent(req, file, buffer);
	}

	if (sock->write(buffer.data(), buffer.size()) == 0)
	{
		return -1;
	}

	return 0;
}

bool FSRequestProcessor::isEligible(const HttpRequest& req) const
{
	(void)req;
	return true;
}
