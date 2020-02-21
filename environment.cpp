#include "environment.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

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

std::function<void(const std::vector<uint8_t>&)> lastCallback;
void readFile(uint64_t addr, size_t len){
  uint8_t* data = reinterpret_cast<uint8_t*>(addr);
  std::vector<uint8_t> lastUpload;
  for (size_t i = 0; i < len; i++) {
	lastUpload.push_back(data[i]);
  }
  lastCallback(lastUpload);
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("readFile", &readFile);
}

void uploadFile(std::function<void(const std::vector<uint8_t>&)> callback) {
	std::string code = "var n = document.createElement('input');"
	"n.type = 'file';"
	"n.onchange = e => { const file = e.currentTarget.files[0];"
		"if(!(file instanceof Blob)) return;"
		"const r = new FileReader();"
		"r.onloadend = evt => {"
			"const uint8_t_arr = new Uint8Array(evt.target.result);"
			"const uint8_t_ptr = window.Module._malloc(uint8_t_arr.length);"
			"window.Module.HEAPU8.set(uint8_t_arr, uint8_t_ptr);"
			"window.Module.readFile(uint8_t_ptr, uint8_t_arr.length);"
			"const returnArr = new Uint8Array(uint8_t_arr.length);"
			"returnArr.set(window.Module.HEAPU8.subarray(uint8_t_ptr, uint8_t_ptr + uint8_t_arr.length));"
			"window.Module._free(uint8_t_ptr);"
		"}"
		"r.readAsArrayBuffer(file);"
	"}"
	"n.click();";
	emscripten_run_script(code.c_str());
	lastCallback = callback;
}

void downloadFile(const std::vector<uint8_t>& downloaded) {
	std::string code = "let s=window.atob('" + Serialisable::toBase64(downloaded) + "');"
			"let b=new Uint8Array(s.length);"
			"for(var i=0;i<b.length;i++)"
				"b[i]=s.charCodeAt(i);"
			"let a=document.createElement('a');"
			"document.body.appendChild(a);"
			"a.style='display: none';"
			"let l=new Blob([b],{type:'octet/stream'});"
			"let u=window.URL.createObjectURL(l);"
			"a.href=u;"
			"a.download='file.cjson';"
			"a.click();"
			"window.URL.revokeObjectURL(u);";
	emscripten_run_script(code.c_str());
}

#else

#include <QFileDialog>
#include <iostream>

void uploadFile(std::function<void(const std::vector<uint8_t>&)> callback) {
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Upload", "", "Files (*.*)");
	std::ifstream input(fileName.toStdString(), std::ios::binary);
	std::vector<unsigned char> result(std::istreambuf_iterator<char>(input), {});
	callback(result);
}

void downloadFile(const std::vector<uint8_t>& downloaded) {
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Save", "", "Files (*.*)");
	std::ofstream output(fileName.toStdString(), std::ios::binary);
	output.write(reinterpret_cast<const char*>(&(downloaded[0])), downloaded.size());
}

#endif
