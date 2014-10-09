#pragma once

#include <string>
#include <map>

#include "boost/any.hpp"

namespace myhttp {
namespace service {

class MiddlewareParams {
public:
	void setValue(const std::string &key, const boost::any &value);
	boost::any getValue(const std::string &key) const;
private:
	std::map<std::string, boost::any> m_keyToValue;
};

}
}

