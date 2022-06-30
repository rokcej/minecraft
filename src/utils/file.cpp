#include "file.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace file {

// https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string ReadTextFile(const std::string& path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file) {
		throw std::runtime_error("Failed to open file " + path);
	}
	std::ostringstream contents;
	contents << file.rdbuf();
	return contents.str();
}

} // namespace file
