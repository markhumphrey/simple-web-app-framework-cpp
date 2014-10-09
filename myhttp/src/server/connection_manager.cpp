#include "connection_manager.hpp"

#include <algorithm>
#include <boost/bind.hpp>

namespace myhttp {
namespace server {

void ConnectionManager::start(connection_ptr c) {
	m_connections.insert(c);
	c->start();
}

void ConnectionManager::stop(connection_ptr c) {
	m_connections.erase(c);
	c->stop();
}

void ConnectionManager::stopAll() {
	for (auto c : m_connections) {
		c->stop();
	}
	m_connections.clear();
}

} // namespace server
} // namespace http

