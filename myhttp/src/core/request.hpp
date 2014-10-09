#pragma once

#include <string>
#include <vector>
#include <map>

#include "header.hpp"

namespace myhttp {
namespace core {

class RouteParams {
public:
	void setValue(const std::string &key, const std::string &value);
	std::string getValue(const std::string &key) const;
private:
	std::map<std::string, std::string> m_keyToValue;
};

class QueryParams {
public:
	void setValue(const std::string &key, const std::string &value);
	std::string getValue(const std::string &key) const;
private:
	std::map<std::string, std::string> m_keyToValue;
};

class HeadersInfo {
public:
};

class Request {
public:
	const std::string& getVerb() const {
		return m_method;
	}
	void setVerb(const std::string& verb) {
		m_method = verb;
	}

	const std::string getPath() const;
	void setPath(const std::string &path) {
		m_uri = path;
	}

	// object containing properties mapped to the named route "parameters"
	const RouteParams& getParams() const {
		return m_routeParams;
	}
	void setParams(const RouteParams &routeParams) {
		m_routeParams = routeParams;
	}

	// object representing the parsed query string
	const QueryParams& getQuery() const {
		return m_queryParams;
	}
	void setQuery(const QueryParams &queryParams) {
		m_queryParams = queryParams;
	}

	// object representing case insensitive request headers
	//const HeadersInfo& getHeaders() const;
	const std::vector<Header> getHeaders() const {
		return m_headers;
	}
	void setHeaders(const std::vector<Header> &headers) {
		m_headers = headers;
	}

private:
	std::string m_method;
	std::string m_uri;
	int m_httpVersionMajor;
	int m_httpVersionMinor;
	std::vector<Header> m_headers;

	RouteParams m_routeParams;
	QueryParams m_queryParams;
};

}
}

