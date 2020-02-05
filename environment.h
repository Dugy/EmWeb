#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>
#include <serialisable/serialisable.hpp>

#ifdef __EMSCRIPTEN__

#include "emscripten.h"
std::string getFile(const std::string& name) {
	std::string fileName = "https://github.com/Dugy/emweb/resources/" + name;
	int size = -1;
	char* data = nullptr;
	int errorCode = 0;
	emscripten_wget_data(name.c_str(), &data, &size, &errorCode);
	if (errorCode or size == 1 || !data)
		throw std::runtime_error(fileName + ": could not get resource");
	std::string made(data);
	free(data);
	return made;
}

std::shared_ptr<Serialisable::JSON> sessionSettings() {
	return std::shared_ptr<Serialisable::JSON>();
}

void sessionSettings(std::shared_ptr<Serialisable::JSON>) {
}

#else

#include <fstream>
std::string getFile(const std::string& name) {
	std::string fileName = "resources/" + name;
	std::ifstream file(fileName);
	if (!file.good()) throw(std::runtime_error(fileName + ": file unavailable"));
	std::string made;

	file.seekg(0, std::ios::end);
	made.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	made.assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
	return made;
}

inline constexpr char* SETTINGS_FILE = "settings.json";

std::shared_ptr<Serialisable::JSON> sessionSettings() {
	std::ifstream file(SETTINGS_FILE);
	return Serialisable::parseJSON(file);
}

void sessionSettings(std::shared_ptr<Serialisable::JSON> from) {
	std::ofstream file(SETTINGS_FILE);
	from->write(file);
}

#endif

#endif // ENVIRONMENT_H
