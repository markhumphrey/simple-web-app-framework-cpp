#pragma once

#include <set>
#include <boost/noncopyable.hpp>

#include "connection.hpp"

namespace myhttp {
namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ConnectionManager: private boost::noncopyable {
public:
	/// Add the specified connection to the manager and start it.
	void start(connection_ptr c);

	/// Stop the specified connection.
	void stop(connection_ptr c);

	/// Stop all connections.
	void stopAll();

private:
	/// The managed connections.
	std::set<connection_ptr> m_connections;
};

} // namespace server
} // namespace http
