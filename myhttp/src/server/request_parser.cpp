#include "request_parser.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <istream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace myhttp::core;

namespace myhttp {
namespace server {

void RequestParser::parseRequestLine(std::istream &response_stream) {
	std::string line;
	std::getline(response_stream, line);

	const std::string regexStr = R"(^\s*(\w+)\s*(\S+)\s*HTTP\/(\S+)\s*\r$)";
	boost::regex regex(regexStr);
	boost::smatch match;
	boost::regex_search(line, match, regex);

	if (match.size() != 4) {
		cerr << "Invalid response" << endl;
		return;
	}
	const string verb = match[1].str();
	m_request.setVerb(verb);
	const string path = match[2].str();
	m_request.setPath(path);
	const string httpVersion = match[3].str();
}

void RequestParser::parseHeaders(std::istream &response_stream) {
	// Process the response headers.;
	vector<Header> headers;
	std::string header;
	while (std::getline(response_stream, header) && header != "\r") {
		//split header into name and value
		const std::string regexStr = R"(^\s*([^:]+):\s*([^\r]+)\r$)";
		boost::regex regex(regexStr);
		boost::smatch match;
		boost::regex_search(header, match, regex);

		headers.push_back(Header());
		// full header at index 0
		if (match.size() > 1) {
			// key at index 1
			headers.back().m_name = match[1].str();
		}
		if (match.size() > 2) {
			// value at index 2
			headers.back().m_value = match[2].str();
		}

	}

	m_request.setHeaders(headers);
}

void RequestParser::parseContent(std::istream &response_stream) {
	// add all the data which has been read so far
	std::string content;
	response_stream >> content;
	//TODO - add content parsing and add to response
	//m_response.setContent(m_response.getContent() + content);
}

}
}

