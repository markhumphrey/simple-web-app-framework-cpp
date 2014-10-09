#pragma once

#include <functional>
#include <string>
#include <vector>

#include "middleware_params.hpp"
#include "request.hpp"
#include "response.hpp"
#include "route.hpp"
#include "verb_handler.hpp"

namespace myhttp {
namespace service {

class Resource {
public:
	Resource(const Route &route);

	const Route &getRoute() const {
		return m_route;
	}

	void addAll(const VerbHandler &handler);
	void addGet(const VerbHandler &handler);
	void addPut(const VerbHandler &handler);
	void addPost(const VerbHandler &handler);
	void addDelete(const VerbHandler &handler);

	bool handleRequest(const core::Request& req, core::Response& rep,
			MiddlewareParams& params) const;

private:

	Route m_route;

	// verb handlers in order of execution
	std::vector<VerbHandler> m_allHandlers;
	std::vector<VerbHandler> m_getHandlers;
	std::vector<VerbHandler> m_putHandlers;
	std::vector<VerbHandler> m_postHandlers;
	std::vector<VerbHandler> m_deleteHandlers;
};

}
}

