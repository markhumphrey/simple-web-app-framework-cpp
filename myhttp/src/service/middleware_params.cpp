#include "middleware_params.hpp"

namespace myhttp {
namespace service {

void MiddlewareParams::setValue(const std::string &key,
		const boost::any &value) {
	m_keyToValue[key] = value;
}

boost::any MiddlewareParams::getValue(const std::string &key) const {
	boost::any value;
	try {
		value = m_keyToValue.at(key);
	} catch (const std::out_of_range& e) {
	}
	return value;
}

}
}
