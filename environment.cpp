#include "environment.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

std::string getFile(const std::string& name) {
	std::string script = "let r = new XMLHttpRequest();";
	script += "r.open('GET', 'https://raw.githubusercontent.com/Dugy/EmWeb/master/resources/" + name + "', false);";
	script += "r.send(null);";
	script += "r.responseText;";
	char* got = emscripten_run_script_string(script.c_str());
	std::string obtained = got;
	return obtained;
}

std::shared_ptr<Serialisable::JSON> sessionSettings() {
	std::string got = emscripten_run_script_string("window.location.href");
	auto binary = Serialisable::fromBase64(got.substr(got.find_last_of('?') + 1));
	return Serialisable::parseCondensed(binary);
}

void sessionSettings(const std::shared_ptr<Serialisable::JSON>& settings) {
	auto binary = settings->condensed();
	std::string script = "history.pushState(null, null, window.location.pathname + '?' + '" + Serialisable::toBase64(binary) + "');";
	emscripten_run_script(script.c_str());
}
#endif
