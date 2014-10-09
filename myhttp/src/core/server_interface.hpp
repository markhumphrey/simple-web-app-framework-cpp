#pragma once

#include <string>

namespace myhttp {
namespace core {

class Request;
class Response;

class ServerInterface {
public:
	virtual ~ServerInterface() {
	}
	;

	virtual void setAddress(const std::string &address,
			const std::string &port) = 0;
	typedef std::function<bool(const core::Request&, core::Response&)> RequestHandler;
	virtual void setRequestHandler(const RequestHandler &requestHandler) = 0;

	virtual void run() = 0;
	virtual void stop() = 0;
};

}
}
