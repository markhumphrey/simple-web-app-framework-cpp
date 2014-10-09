#include "resource.hpp"

#include <boost/regex.hpp>

using namespace myhttp::core;

namespace myhttp {
namespace service {

Resource::Resource(const Route &route) :
		m_route(route) {

}

void Resource::addAll(const VerbHandler &handler) {
	m_allHandlers.push_back(handler);
}

void Resource::addGet(const VerbHandler &handler) {
	m_getHandlers.push_back(handler);
}

void Resource::addPut(const VerbHandler &handler) {
	m_putHandlers.push_back(handler);
}

void Resource::addPost(const VerbHandler &handler) {
	m_postHandlers.push_back(handler);
}

void Resource::addDelete(const VerbHandler &handler) {
	m_deleteHandlers.push_back(handler);
}

bool Resource::handleRequest(const core::Request& req, core::Response& rep,
		MiddlewareParams &params) const {
	// if any of the handlers return false, don't continue
	// execute the ALL verb handlers
	for (auto i = m_allHandlers.begin(); i != m_allHandlers.end(); ++i) {
		if (!(*i)(req, rep, params)) {
			return false;
		}
	}

	// execute appropriate verb handlers
	if (req.getVerb() == "GET") {
		for (auto i = m_getHandlers.begin(); i != m_getHandlers.end(); ++i) {
			if (!(*i)(req, rep, params)) {
				return false;
			}
		}
	} else if (req.getVerb() == "PUT") {
		for (auto i = m_putHandlers.begin(); i != m_putHandlers.end(); ++i) {
			if (!(*i)(req, rep, params)) {
				return false;
			}
		}
	} else if (req.getVerb() == "POST") {
		for (auto i = m_postHandlers.begin(); i != m_postHandlers.end(); ++i) {
			if (!(*i)(req, rep, params)) {
				return false;
			}
		}
	} else if (req.getVerb() == "DELETE") {
		for (auto i = m_deleteHandlers.begin(); i != m_deleteHandlers.end();
				++i) {
			if (!(*i)(req, rep, params)) {
				return false;
			}
		}
	}

	return false;
}

} // end namespace service
} // end namespace http

