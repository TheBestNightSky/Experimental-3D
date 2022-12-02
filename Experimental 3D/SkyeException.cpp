#include "SkyenetException.h"
#include "Window.h"
#include <sstream>

SkyenetException::SkyenetException(int line, const char* file) noexcept : line(line), file(file) {}

const char* SkyenetException::what() const noexcept {

	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* SkyenetException::GetType() const noexcept {

	return "Skyenet Exception";

}

const std::string& SkyenetException::GetFile() const noexcept {

	return file;

}

std::string SkyenetException::GetOriginString() const noexcept {

	std::ostringstream oss;
	oss << "[File] " << file << std::endl << "[Line] " << line;
	return oss.str();

}