#include "request.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <string>
#include <sstream>

bool url_decode(const std::string& in, std::string& out) {
	out.clear();
	out.reserve(in.size());
	for (std::size_t i = 0; i < in.size(); ++i) {
		if (in[i] == '%') {
			if (i + 3 <= in.size()) {
				int value = 0;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value) {
					out += static_cast<char>(value);
					i += 2;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if (in[i] == '+') {
			out += ' ';
		} else {
			out += in[i];
		}
	}
	return true;
}

namespace myhttp {
namespace core {

const std::string Request::getPath() const {
	std::string decoded = m_uri;
	url_decode(m_uri, decoded);
	return decoded;
}

void RouteParams::setValue(const std::string &key, const std::string &value) {
	m_keyToValue[key] = value;
}

std::string RouteParams::getValue(const std::string &key) const {
	std::string value;
	try {
		value = m_keyToValue.at(key);
	} catch (const std::out_of_range& e) {
	}
	return value;
}

void QueryParams::setValue(const std::string &key, const std::string &value) {
	m_keyToValue[key] = value;
}

std::string QueryParams::getValue(const std::string &key) const {
	std::string value;
	try {
		value = m_keyToValue.at(key);
	} catch (const std::out_of_range& e) {
	}
	return value;
}

}
}
