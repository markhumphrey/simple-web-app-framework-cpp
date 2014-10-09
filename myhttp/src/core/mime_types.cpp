#include "mime_types.hpp"
#include <unordered_map>

namespace myhttp {
namespace server {
namespace mime_types {

const std::unordered_map<std::string, std::string> extToType = { { "gif",
		"image/gif" }, { "htm", "text/html" }, { "html", "text/html" }, { "jpg",
		"image/jpeg" }, { "png", "image/png" }, };

std::string extension_to_type(const std::string& extension) {
	auto i = extToType.find(extension);
	if (i != extToType.end()) {
		return i->second;
	}
	return "text/plain";
}

} // namespace mime_types
} // namespace server
} // namespace http

