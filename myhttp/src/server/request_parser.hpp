#pragma once

#include <istream>

#include "request.hpp"

namespace myhttp {
namespace server {

class RequestParser {
public:
	void reset();

	void parseRequestLine(std::istream &response_stream);
	void parseHeaders(std::istream &response_stream);
	void parseContent(std::istream &response_stream);

	const myhttp::core::Request &getRequest() const {
		return m_request;
	}

private:
	myhttp::core::Request m_request;
};

}
}

