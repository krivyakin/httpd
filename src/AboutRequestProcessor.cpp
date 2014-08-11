#include <sstream>

#include "AboutRequestProcessor.h"

AboutRequestProcessor::AboutRequestProcessor(ServerPtr server)
: server_(server)
{

}

int AboutRequestProcessor::process(HttpRequest& req, SocketPtr sock)
{
	std::stringstream ss;

	std::vector<Worker::Stat> stats;
	server_->stat(stats);

	ss << "<table><tr><td>#</td><td>Current</td><td>Total</td></tr>";
	for (auto it = stats.cbegin(); it != stats.cend(); ++it)
	{
		ss << "<tr><td>worker #" << it - stats.cbegin() <<
		"</td><td>" << it->currentConnections << "</td>" <<
			"<td>" << it->totalConnections << "</td></tr>";
	}
	ss << "</table>";

	std::string content = ss.str();

	Buffer<uint8_t> buffer;
	makeHttpResponse(req, content.data(), content.size(), buffer);

	if (sock->write(buffer.data(), buffer.size()) == 0)
	{
		return -1;
	}

	(void)req;
	return 0;
}

bool AboutRequestProcessor::isEligible(const HttpRequest& req) const
{
	if (req.path == "/about")
		return true;

	return false;
}
