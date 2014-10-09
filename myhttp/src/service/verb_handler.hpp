#pragma once

#include <functional>

namespace myhttp {
namespace core {

class Request;
class Response;

}

namespace service {

class MiddlewareParams;

typedef std::function<
		bool(const core::Request&, core::Response&, MiddlewareParams&)> VerbHandler;

}
}

