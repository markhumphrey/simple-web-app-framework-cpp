#pragma once

#include <functional>
#include <map>

#include "resource.hpp"
#include "verb_handler.hpp"

namespace myhttp {
namespace core {
class Request;
class Response;
class ServerInterface;
}
}

namespace myhttp {
namespace service {

class Application {
public:
	Application(std::shared_ptr<core::ServerInterface> server);

	void addMiddleware(const VerbHandler &handler);
	void addResource(const Resource &res);

	bool handleRequest(const core::Request& req, core::Response& rep);

	void listen(int port);

private:

	std::shared_ptr<core::ServerInterface> m_server;

	std::vector<VerbHandler> m_middleware;
	std::vector<Resource> m_resources;
};

}
}

