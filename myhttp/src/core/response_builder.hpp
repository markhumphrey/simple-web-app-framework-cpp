#pragma once

#include <map>

#include "response.hpp"

namespace myhttp {
namespace core {

class ResponseBuilder {
public:
	Response build();

	ResponseBuilder& setStatus(Response::status_type status);

	ResponseBuilder& setHeader(const std::string &field,
			const std::string &value);

	ResponseBuilder& setContent(const std::string &content);
	ResponseBuilder& setContent(const std::vector<std::string> &content);
	ResponseBuilder& setContent(
			const std::map<std::string, std::string> &content);
	ResponseBuilder& setFilenameContent(const std::string &filename);

	ResponseBuilder& setDefaultResponse(Response::status_type status);

private:
	Response m_response;
	std::map<std::string, std::string> m_headers;
};

}
}

