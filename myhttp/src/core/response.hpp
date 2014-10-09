#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"

namespace myhttp {
namespace core {

/// A reply to be sent to a client.
class Response {
public:
	/// The status of the reply.
	enum status_type {
		ok = 200,
		created = 201,
		accepted = 202,
		no_content = 204,
		multiple_choices = 300,
		moved_permanently = 301,
		moved_temporarily = 302,
		not_modified = 304,
		bad_request = 400,
		unauthorized = 401,
		forbidden = 403,
		not_found = 404,
		internal_server_error = 500,
		not_implemented = 501,
		bad_gateway = 502,
		service_unavailable = 503
	};

	status_type getStatus() const {
		return m_status;
	}
	void setStatus(status_type status) {
		m_status = status;
	}

	const std::vector<Header> & getHeaders() const {
		return m_headers;
	}
	void setHeaders(const std::vector<Header> &headers) {
		m_headers = headers;
	}

	const std::string& getContent() const {
		return m_content;
	}
	void setContent(const std::string &content) {
		m_content = content;
	}

	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> toBuffers();

private:
	status_type m_status;
	/// The headers to be included in the reply.
	std::vector<Header> m_headers;
	/// The content to be sent in the reply.
	std::string m_content;
};

} // namespace server
} // namespace http

