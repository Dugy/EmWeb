#include <iostream>
#include <string>
#include <iterator>
#include "cfg_generator.hpp"

int main(int argc, char** argv) {
	srand(time(nullptr));
	CfgGenerator gen;
	if (argc < 2) {
		std::cin >> std::noskipws;
		std::istream_iterator<char> it(std::cin);
		std::istream_iterator<char> end;
		std::string source(it, end);
		try {
			gen.constructFromString(source);
		} catch (std::exception& error) {
			std::cerr << "CFG Generator failed: " << error.what() << std::endl;
			return 2;
		}
	} else {
		for (int i = 1; i < argc; i++) {
			try {
				gen.constructFromFile(argv[i]);
			} catch (std::exception& error) {
				std::cerr << "CFG Generator failed with file " << argv[i] << ": " << error.what() << std::endl;
				return 2;
			}
		}
	}
	std::string got = gen.generate(rand());
	std::cout << got << std::endl;
	return 0;
}
