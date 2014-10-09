#pragma once

#include <string>

namespace myhttp {
namespace core {

struct Header {
	Header() {
	}
	Header(const std::string &name, const std::string &value) :
			m_name(name), m_value(value) {
	}
	std::string m_name;
	std::string m_value;
};

}
}

