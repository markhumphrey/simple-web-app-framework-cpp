#include "response_builder.hpp"

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <unordered_map>

#include "mime_types.hpp"

using namespace std;

namespace myhttp {
namespace core {

Response ResponseBuilder::build() {
	vector<Header> headers;
	for (map<string, string>::const_iterator i = m_headers.begin();
			i != m_headers.end(); ++i) {
		headers.push_back(Header(i->first, i->second));
	}
	m_response.setHeaders(headers);
	return m_response;
}

ResponseBuilder& ResponseBuilder::setStatus(Response::status_type status) {
	m_response.setStatus(status);
	return *this;
}

ResponseBuilder& ResponseBuilder::setHeader(const std::string &field,
		const std::string &value) {
	m_headers[field] = value;
	return *this;
}

ResponseBuilder& ResponseBuilder::setContent(const std::string &content) {
	m_response.setContent(content);
	return *this;
}

ResponseBuilder& ResponseBuilder::setContent(
		const std::vector<std::string> &content) {
	std::string contentStr = "[ ";
	for (unsigned int i = 0; i < content.size(); ++i) {
		contentStr += "\"" + content[i] + "\", ";
	}
	// TOD - extra comma
	contentStr += " ]";
	m_response.setContent(contentStr);
	return *this;
}

ResponseBuilder& ResponseBuilder::setContent(
		const std::map<std::string, std::string> &content) {
	std::string contentStr = "{ ";
	for (std::map<std::string, std::string>::const_iterator i = content.begin();
			i != content.end(); ++i) {
		contentStr += "\"" + i->first + "\": \"" + i->second + "\",";
	}
	// TODO - extra comma
	contentStr += " }";
	m_response.setContent(contentStr);
	return *this;
}

ResponseBuilder& ResponseBuilder::setFilenameContent(
		const std::string &filename) {

	// Request path must be absolute and not contain "..".
	if (filename.empty() || filename[0] != '/'
			|| filename.find("..") != std::string::npos) {
		// TODO - add error handling for relative request path
		//rep = ResponseBuilder().setDefaultResponse(Response::bad_request).build();
		return *this;
	}

	// Determine the file extension.
	std::size_t last_slash_pos = filename.find_last_of("/");
	std::size_t last_dot_pos = filename.find_last_of(".");
	std::string extension;
	if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
		extension = filename.substr(last_dot_pos + 1);
	}

	// Open the file to send back.
	//std::string full_path = doc_root_ + request_path;
	std::string full_path = filename;
	std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
	if (!is) {
		// TODO - add error handling for missing file
		//rep =().setDefaultResponse(Response::bad_request).build();
	}

	// Fill out the Response to be sent to the client.
	char buf[512];
	std::string content;
	while (is.read(buf, sizeof(buf)).gcount() > 0)
		content.append(buf, is.gcount());
	m_response.setContent(content);

	setHeader("Content-Length",
			boost::lexical_cast<std::string>(content.size()));
	setHeader("Content-Type",
			myhttp::server::mime_types::extension_to_type(extension));

	return *this;
}

namespace stock_replies {

const std::unordered_map<Response::status_type, std::string, std::hash<int> > statusToContent =
		{ { Response::ok, "" }, { Response::created, "<html>"
				"<head><title>Created</title></head>"
				"<body><h1>201 Created</h1></body>"
				"</html>" }, { Response::accepted, "<html>"
				"<head><title>Accepted</title></head>"
				"<body><h1>202 Accepted</h1></body>"
				"</html>" }, { Response::no_content, "<html>"
				"<head><title>No Content</title></head>"
				"<body><h1>204 Content</h1></body>"
				"</html>" }, { Response::multiple_choices, "<html>"
				"<head><title>Multiple Choices</title></head>"
				"<body><h1>300 Multiple Choices</h1></body>"
				"</html>" }, { Response::moved_permanently, "<html>"
				"<head><title>Moved Permanently</title></head>"
				"<body><h1>301 Moved Permanently</h1></body>"
				"</html>" }, { Response::moved_temporarily, "<html>"
				"<head><title>Moved Temporarily</title></head>"
				"<body><h1>302 Moved Temporarily</h1></body>"
				"</html>" }, { Response::not_modified, "<html>"
				"<head><title>Not Modified</title></head>"
				"<body><h1>304 Not Modified</h1></body>"
				"</html>" }, { Response::bad_request, "<html>"
				"<head><title>Bad Request</title></head>"
				"<body><h1>400 Bad Request</h1></body>"
				"</html>" }, { Response::unauthorized, "<html>"
				"<head><title>Unauthorized</title></head>"
				"<body><h1>401 Unauthorized</h1></body>"
				"</html>" }, { Response::forbidden, "<html>"
				"<head><title>Forbidden</title></head>"
				"<body><h1>403 Forbidden</h1></body>"
				"</html>" }, { Response::not_found, "<html>"
				"<head><title>Not Found</title></head>"
				"<body><h1>404 Not Found</h1></body>"
				"</html>" }, { Response::internal_server_error, "<html>"
				"<head><title>Internal Server Error</title></head>"
				"<body><h1>500 Internal Server Error</h1></body>"
				"</html>" }, { Response::not_implemented, "<html>"
				"<head><title>Not Implemented</title></head>"
				"<body><h1>501 Not Implemented</h1></body>"
				"</html>" }, { Response::bad_gateway, "<html>"
				"<head><title>Bad Gateway</title></head>"
				"<body><h1>502 Bad Gateway</h1></body>"
				"</html>" }, { Response::service_unavailable, "<html>"
				"<head><title>Service Unavailable</title></head>"
				"<body><h1>503 Service Unavailable</h1></body>"
				"</html>" } };

std::string to_string(Response::status_type status) {
	auto i = statusToContent.find(status);
	if (i != statusToContent.end()) {
		return i->second;
	}
	return statusToContent.find(Response::internal_server_error)->second;
}

}

ResponseBuilder& ResponseBuilder::setDefaultResponse(
		Response::status_type status) {
	Response rep;
	rep.setStatus(status);
	rep.setContent(stock_replies::to_string(status));
	std::vector<Header> headers;
	headers.resize(2);
	headers[0].m_name = "Content-Length";
	headers[0].m_value = boost::lexical_cast<std::string>(
			rep.getContent().size());
	headers[1].m_name = "Content-Type";
	headers[1].m_value = "text/html";
	rep.setHeaders(headers);
	//return rep;
	return *this;
}

}
}
