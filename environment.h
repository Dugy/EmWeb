#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>
#include <serialisable/serialisable.hpp>

#ifdef __EMSCRIPTEN__

std::string getFile(const std::string& name);

std::shared_ptr<Serialisable::JSON> sessionSettings();

void sessionSettings(const std::shared_ptr<Serialisable::JSON>& settings);

#else

#include <fstream>
inline std::string getFile(const std::string& name) {
	std::string fileName = "../resources/" + name;
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

inline constexpr char const* SETTINGS_FILE = "settings.json";

inline std::shared_ptr<Serialisable::JSON> sessionSettings() {
	std::ifstream file(SETTINGS_FILE);
	return Serialisable::parseJSON(file);
}

inline void sessionSettings(const std::shared_ptr<Serialisable::JSON>& from) {
	std::ofstream file(SETTINGS_FILE);
	from->write(file);
}

#endif

#endif // ENVIRONMENT_H
