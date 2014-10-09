#include "application.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include "middleware_params.hpp"
#include "mime_types.hpp"
#include "response.hpp"
#include "request.hpp"
#include "server_interface.hpp"
#include "verb_handler.hpp"

using namespace std;
using namespace myhttp::core;
using namespace myhttp::server;

namespace myhttp {
namespace service {

Application::Application(std::shared_ptr<core::ServerInterface> server) :
		m_server(server) {

}

void Application::addMiddleware(const VerbHandler &handler) {
	m_middleware.push_back(handler);
}

void Application::addResource(const Resource &res) {
	m_resources.push_back(res);
}

bool Application::handleRequest(const core::Request& req, core::Response& rep) {

	// run middleware
	MiddlewareParams params;
	for (const VerbHandler &m : m_middleware) {
		// if any of the middleware handles return false don't continue
		if (!m(req, rep, params)) {
			return false;
		}
	}

	// dispatch to matching resource
	for (const Resource &r : m_resources) {
		RouteParams routeParams;
		QueryParams queryParams;
		if (r.getRoute().isMatch(req.getPath(), routeParams, queryParams)) {
			Request myReq = req;
			myReq.setParams(routeParams);
			myReq.setQuery(queryParams);
			r.handleRequest(myReq, rep, params);
			break;
		}
	}
	return true;
}

void Application::listen(int port) {

	try {
		const string address("127.0.0.1");
		m_server->setAddress(address, std::to_string(port));
		core::ServerInterface::RequestHandler rh = std::bind(
				&Application::handleRequest, this, std::placeholders::_1,
				std::placeholders::_2);
		m_server->setRequestHandler(rh);
		m_server->run();
	} catch (std::exception &e) {
		std::cerr << "exception: " << e.what() << endl;
	}

}

}
}
