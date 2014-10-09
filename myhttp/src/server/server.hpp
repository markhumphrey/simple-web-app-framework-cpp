#pragma once

#include <boost/asio.hpp>
#include <string>

#include "connection.hpp"
#include "connection_manager.hpp"
#include "server_interface.hpp"

namespace myhttp {
namespace server {

class Server: public core::ServerInterface {
public:
	Server();
	virtual ~Server();

	virtual void setAddress(const std::string &address, const std::string &port)
			override;
	virtual void setRequestHandler(
			const core::ServerInterface::RequestHandler &requestHandler)
					override;

	virtual void run() override;
	virtual void stop() override;

private:
	void initAcceptor();
	// start an async accept operation
	void startAccept();
	// handle completion of async accept operation
	void handleAccept(const boost::system::error_code& e);
	// handle a request to stop the server
	void handleStop();

	std::string m_address;
	std::string m_port;

	RequestHandler m_requestHandler;

	// the io_service used to perform asynchronous operations
	boost::asio::io_service m_ioService;
	// acceptor used to listen for incoming connections
	boost::asio::ip::tcp::acceptor m_acceptor;

	ConnectionManager m_connectionManager;

	// connection manager which owns all connections
	boost::shared_ptr<Connection> m_connection;

};

}
}

