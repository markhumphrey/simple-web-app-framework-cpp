#pragma once

#include <boost/regex.hpp>
#include <string>
#include <vector>

#include "request.hpp"

namespace myhttp {
namespace service {

class Route {
public:
	Route(const std::string &path);

	const std::string &getPath() const {
		return m_path;
	}
	const std::vector<std::string> &getPathKeys() const {
		return m_pathKeys;
	}

	bool isMatch(const std::string &url, core::RouteParams &routeParams,
			core::QueryParams &queryParams) const;

private:

	void initRegex();

	std::string m_path;
	boost::regex m_pathRegex;
	std::vector<std::string> m_pathKeys;
};

}
}
