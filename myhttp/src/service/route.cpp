#include "route.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace myhttp {
namespace service {

Route::Route(const std::string &path) :
		m_path(path) {
	initRegex();
}

void parseQuery(const std::string &query, std::vector<std::string> &keys,
		std::vector<std::string> &values) {
	keys.clear();
	values.clear();

	std::vector < std::string > fields;
	boost::split(fields, query, boost::is_any_of("&"));

	for (size_t i = 0; i < fields.size(); ++i) {
		const string &field = fields[i];
		size_t found = field.find_first_of("=");

		// TODO - handle decoding query field and value

		if (found == string::npos) {
			keys.push_back(field);
			values.push_back("");
		} else {
			keys.push_back(field.substr(0, found));
			values.push_back(field.substr(found + 1));
		}
	}

}

void splitPath(const std::string &url, std::string &path, std::string &query) {
	path.clear();
	query.clear();

	//the query component is indicated by the first '?' and terminated by '#' or the end of the URI
	size_t found = url.find_first_of("?");
	if (found == string::npos) {
		path = url;
	} else {
		path = url.substr(0, found);
		query = url.substr(found + 1);
	}

}

bool Route::isMatch(const std::string &url, core::RouteParams &routeParams,
		core::QueryParams &queryParams) const {
	vector < string > pathValues;
	pathValues.clear();

	//split url into path and query string
	std::string path;
	std::string query;
	splitPath(url, path, query);

	//check if path matches this route
	boost::smatch placeMatch;
	if (!boost::regex_search(path, placeMatch, m_pathRegex)) {
		return false;
	}

	// extract url placeholder values
	for (size_t i = 1; i < placeMatch.size(); ++i) {
		boost::ssub_match subMatch = placeMatch[i];
		pathValues.push_back(subMatch.str());
	}

	for (size_t i = 0; i < m_pathKeys.size(); ++i) {
		routeParams.setValue(m_pathKeys[i], pathValues[i]);
	}

	// exract query string keys and values
	std::vector < string > keys;
	std::vector < string > values;
	parseQuery(query, keys, values);

	for (size_t i = 0; i < keys.size(); ++i) {
		queryParams.setValue(keys[i], values[i]);
	}

	return true;
}

void Route::initRegex() {
	m_pathKeys.clear();

	// std::regex does not work in gcc 4.8.* grrr...
	// construct regex from path
	// also extracting placeholder keys
	string regexStr = "^";

	const std::string placeRegexStr = R"(\/)";
	const boost::regex placeRegex(placeRegexStr);
	boost::sregex_token_iterator i(m_path.begin(), m_path.end(), placeRegex,
			-1);
	boost::sregex_token_iterator j;
	while (i != j) {
		std::string token = *i;
		++i;
		if (!token.empty()) {
			if (token[0] == ':') {
				regexStr +=
						R"(/(\w*))";
				// add token to list of keys, chopping off ":"
				m_pathKeys.push_back(token.substr(1));
			}
			else {
				regexStr += "/" + token;
			}
		}
	}
	// if path had trailing slash then include in matching regex
	if (m_path.back() == '/') {
		regexStr += "/";
	}
	regexStr += "$";

	m_pathRegex = boost::regex(regexStr);
}

}
}

