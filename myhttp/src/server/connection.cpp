#include "connection.hpp"

#include <boost/bind.hpp>

#include "connection_manager.hpp"
#include "response_builder.hpp"

using namespace std;
using namespace myhttp::core;
using namespace myhttp::server;

Connection::Connection(boost::asio::io_service &ioService,
		ConnectionManager &connectionManager,
		core::ServerInterface::RequestHandler &requestHandler) :
		m_socket(ioService), m_connectionManager(connectionManager), m_requestHandler(
				requestHandler) {
}

void Connection::start() {
	startRead();
}

void Connection::stop() {
	m_socket.close();
}

boost::asio::ip::tcp::socket& Connection::socket() {
	return m_socket;
}

void Connection::startRead() {
	// read each part of http request starting with the headers
	startReadRequestLine();
}

void Connection::startReadRequestLine() {
	boost::asio::async_read_until(m_socket, m_buffer, "\r\n",
			boost::bind(&Connection::handleReadRequestLine, this,
					boost::asio::placeholders::error));
}

void Connection::handleReadRequestLine(const boost::system::error_code& err) {
	if (!err) {
		std::istream response_stream(&m_buffer);
		m_requestParser.parseRequestLine(response_stream);

		startReadHeaders();
	} else {
		cerr << "Error: " << err << "\n";
	}
}

void Connection::startReadHeaders() {
	// Read the response headers, which are terminated by a blank line.
	//boost::asio::streambuf::mutable_buffers_type buff = m_buffer.prepare(8192);
	boost::asio::async_read_until(m_socket, m_buffer, "\r\n\r\n",
			boost::bind(&Connection::handleReadHeaders, this,
					boost::asio::placeholders::error));
}

void Connection::handleReadHeaders(const boost::system::error_code& err) {
	if (!err) {
		// Process the response headers.
		std::istream response_stream(&m_buffer);
		m_requestParser.parseHeaders(response_stream);

		// TODO - add content parsing support and push very handling back until after
		// startReadContent();
		m_request = m_requestParser.getRequest();
		m_requestHandler(m_request, m_response);
		startWrite();

	} else {
		cerr << "Error: " << err << "\n";

		m_response =
				ResponseBuilder().setDefaultResponse(Response::bad_request).build();
		startWrite();
	}
}

void Connection::startReadContent() {
	// Continue reading remaining data until EOF.
	boost::asio::async_read(m_socket, m_buffer,
			boost::asio::transfer_at_least(1),
			boost::bind(&Connection::handleReadContent, this,
					boost::asio::placeholders::error));
}

void Connection::handleReadContent(const boost::system::error_code& err) {
	if (!err) {
		// Write all of the data that has been read so far.
		std::istream response_stream(&m_buffer);
		m_requestParser.parseContent(response_stream);

		// Continue reading remaining data until EOF.
		startReadContent();

		m_requestHandler(m_request, m_response);
		startWrite();
	} else if (err != boost::asio::error::eof) {
		cerr << "Error: " << err << "\n";

		m_response =
				ResponseBuilder().setDefaultResponse(Response::bad_request).build();
		startWrite();
	}
}

void Connection::startWrite() {
	boost::asio::async_write(m_socket, m_response.toBuffers(),
			boost::bind(&Connection::handleWrite, shared_from_this(),
					boost::asio::placeholders::error));
}

void Connection::handleWrite(const boost::system::error_code& e) {
	if (!e) {
		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
				ignored_ec);
	}

	if (e != boost::asio::error::operation_aborted) {
		m_connectionManager.stop(shared_from_this());
	}
}
