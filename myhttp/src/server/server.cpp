#include <boost/bind.hpp>

#include "connection_manager.hpp"
#include "server.hpp"

using namespace myhttp::server;

Server::Server() :
		m_address(), m_port(), m_requestHandler(), m_ioService(), m_acceptor(
				m_ioService), m_connectionManager() {
}

Server::~Server() {

}

void Server::setAddress(const std::string &address, const std::string &port) {
	m_address = address;
	m_port = port;
}

void Server::setRequestHandler(const RequestHandler &requestHandler) {
	m_requestHandler = requestHandler;
}

void Server::run() {
	initAcceptor();
	m_ioService.run();
}

void Server::stop() {

}

void Server::initAcceptor() {
	if (m_acceptor.is_open()) {
		return;
	}

	// open the acceptor with the option to reuse the address
	boost::asio::ip::tcp::resolver resolver(m_ioService);
	// the address_configured flag is required to prevent resolve from returning
	// if the loopback device is the only one with an address. basically the third argument is required
	// or boost will throw an exception when resolving localhost
	boost::asio::ip::tcp::resolver::query query(m_address, m_port,
			boost::asio::ip::resolver_query_base::numeric_service);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();

	startAccept();
}

void Server::startAccept() {
	m_connection.reset(
			new Connection(m_ioService, m_connectionManager, m_requestHandler));
	m_acceptor.async_accept(m_connection->socket(),
			boost::bind(&Server::handleAccept, this,
					boost::asio::placeholders::error));
}

void Server::handleAccept(const boost::system::error_code& e) {
	// check if server has already been stopped
	if (!m_acceptor.is_open()) {
		return;
	}

	if (!e) {
		m_connectionManager.start(m_connection);
	}

	startAccept();
}

void Server::handleStop() {
	// the server is stopped by canceling all pending async operations.
	// once all the operations have completed the io_service::run() call will exit
	m_acceptor.close();
	m_connectionManager.stopAll();
}
