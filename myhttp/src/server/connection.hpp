#pragma once

#include <array>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "response.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "server_interface.hpp"

namespace myhttp {
namespace server {

class ConnectionManager;

class Connection: public boost::enable_shared_from_this<Connection>,
		private boost::noncopyable {
public:
	Connection(boost::asio::io_service &ioService,
			ConnectionManager &connectionManager,
			core::ServerInterface::RequestHandler &requestHandler);
	void start();
	void stop();

	// get socket owned by this connection
	boost::asio::ip::tcp::socket &socket();

private:

	void startRead();

	void startReadRequestLine();
	void handleReadRequestLine(const boost::system::error_code& err);

	void startReadHeaders();
	void handleReadHeaders(const boost::system::error_code& err);

	void startReadContent();
	void handleReadContent(const boost::system::error_code& err);

	void startWrite();

	void handleWrite(const boost::system::error_code& e);

	// socket for the connection
	boost::asio::ip::tcp::socket m_socket;

	// connection
	ConnectionManager &m_connectionManager;

	// buffer for incoming data
	//std::array<char, 8192> m_buffer;
	boost::asio::streambuf m_buffer;
	// incoming request
	core::Request m_request;
	core::Response m_response;
	// parser for the incoming request
	RequestParser m_requestParser;
	core::ServerInterface::RequestHandler &m_requestHandler;
};

typedef boost::shared_ptr<Connection> connection_ptr;

}
}

