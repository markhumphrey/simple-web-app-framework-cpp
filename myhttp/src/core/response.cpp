#include "response.hpp"

#include <boost/lexical_cast.hpp>
#include <string>
#include <unordered_map>

namespace myhttp {
namespace core {

namespace status_strings {

const std::unordered_map<Response::status_type, std::string, std::hash<int> > statusToString =
		{ { Response::ok, "HTTP/1.0 200 OK\r\n" }, { Response::created,
				"HTTP/1.0 201 Created\r\n" }, { Response::accepted,
				"HTTP/1.0 202 Accepted\r\n" }, { Response::no_content,
				"HTTP/1.0 204 No Content\r\n" }, { Response::multiple_choices,
				"HTTP/1.0 300 Multiple Choices\r\n" }, {
				Response::moved_permanently,
				"HTTP/1.0 301 Moved Permanently\r\n" }, {
				Response::moved_temporarily,
				"HTTP/1.0 302 Moved Temporarily\r\n" }, {
				Response::not_modified, "HTTP/1.0 304 Not Modified\r\n" }, {
				Response::bad_request, "HTTP/1.0 400 Bad Request\r\n" }, {
				Response::unauthorized, "HTTP/1.0 401 Unauthorized\r\n" }, {
				Response::forbidden, "HTTP/1.0 403 Forbidden\r\n" }, {
				Response::not_found, "HTTP/1.0 404 Not Found\r\n" }, {
				Response::internal_server_error,
				"HTTP/1.0 500 Internal Server Error\r\n" }, {
				Response::not_implemented, "HTTP/1.0 501 Not Implemented\r\n" },
				{ Response::bad_gateway, "HTTP/1.0 502 Bad Gateway\r\n" }, {
						Response::service_unavailable,
						"HTTP/1.0 503 Service Unavailable\r\n" }, };

boost::asio::const_buffer to_buffer(Response::status_type status) {
	auto i = statusToString.find(status);
	if (i != statusToString.end()) {
		return boost::asio::buffer(i->second);
	}
	return boost::asio::buffer(
			statusToString.find(Response::internal_server_error)->second);
}

} // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<boost::asio::const_buffer> Response::toBuffers() {
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(status_strings::to_buffer(m_status));
	for (std::size_t i = 0; i < m_headers.size(); ++i) {
		Header& h = m_headers[i];
		buffers.push_back(boost::asio::buffer(h.m_name));
		buffers.push_back(
				boost::asio::buffer(misc_strings::name_value_separator));
		buffers.push_back(boost::asio::buffer(h.m_value));
		buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	}
	buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	buffers.push_back(boost::asio::buffer(m_content));
	return buffers;
}

} // namespace server
} // namespace http

