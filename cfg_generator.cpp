#include "cfg_generator.hpp"
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <fstream>
#include <stdexcept>

std::string CfgGenerator::PossibilityTerminal::print(seedType& seed, std::unordered_map<Possibility*, seedType>& frozenSeeds,
		std::unordered_map<size_t, unsigned int>& frozenPicks, std::unordered_map<Possibility*, size_t>& frozenPickMap) {
	return _text;
}

std::string CfgGenerator::PossibilityNonterminal::print(seedType& seed, std::unordered_map<Possibility*, seedType>& frozenSeeds,
		std::unordered_map<size_t, unsigned int>& frozenPicks, std::unordered_map<Possibility*, size_t>& frozenPickMap) {
	int picked;
	if (frozenPickMap.find(this) == frozenPickMap.end()) {
		bool seedsEqual = seed == lastSeed;
		lastSeed = seed;
		picked = CfgGenerator::nextRand(seed) % _possibilities.size();
		if (picked == last && !seedsEqual) picked = (picked + 1) % _possibilities.size();
		last = picked;
	} else {
		picked = frozenPicks[frozenPickMap[this]] % _possibilities.size();
	}
	std::vector<Possibility*>& used = _possibilities[picked];
	std::string result;
	auto putTogether = [&] (std::unordered_map<Possibility*, seedType>& seeds, std::unordered_map<size_t, unsigned int>& picks,
			std::unordered_map<Possibility*, size_t>& pickMap) -> void {
		for (unsigned int i = 0; i < used.size(); i++) {
			auto found = frozenSeeds.find(used[i]);
			if (found == frozenSeeds.end()) {
				result += used[i]->print(seed, seeds, picks, pickMap);
			} else {
				seedType subseed = found->second;
				result += used[i]->print(subseed, seeds, picks, pickMap);
			}
		}
	};
	auto selectPicks = [&] (std::unordered_map<Possibility*, seedType>& seeds) -> void {
		if (_picksFreeze.empty()) {
			putTogether(seeds, frozenPicks, frozenPickMap);
		} else {
			std::unordered_map<size_t, unsigned int> newPicks(frozenPicks.begin(), frozenPicks.end());
			std::unordered_map<Possibility*, size_t> newPickMap(frozenPickMap.begin(), frozenPickMap.end());
			for (std::pair<Possibility*, size_t>& poss : _picksFreeze) {
				newPickMap[poss.first] = poss.second;
				auto found = frozenPicks.find(poss.second);
				unsigned int pick = (found != frozenPicks.end()) ? frozenPicks[found->second] : CfgGenerator::nextRand(seed);
				newPicks[poss.second] = pick;
			}
			putTogether(seeds, newPicks, newPickMap);
		}
	};
	if (_seedsFreeze.empty()) {
		selectPicks(frozenSeeds);
	} else {
		std::unordered_map<Possibility*, seedType> newSeeds(frozenSeeds.begin(), frozenSeeds.end());
		for (Possibility* poss : _seedsFreeze) {
			if (newSeeds.find(poss) == newSeeds.end()) {
				newSeeds[poss] = seed;
				CfgGenerator::nextRand(seed);
			}
		}
		selectPicks(newSeeds);
	}
	return result;
}

CfgGenerator::CfgGenerator()
{

}

CfgGenerator::~CfgGenerator()
{

}

short int CfgGenerator::nextRand(int& seed) {
	seed = (214013 * seed + 2531011); 
	return (seed >> 16) & 0x7FFF; 
}

void CfgGenerator::constructFromString(std::string code) {
	const char* reading = code.c_str();
	PossibilityNonterminal* current = nullptr;
	std::vector<Possibility*>* filled = nullptr;
	std::unordered_map<std::string, std::vector<std::string>> _seedsFreeze;
	std::unordered_map<std::string, std::vector<std::pair<std::string, size_t>>> _picksFreeze;
	std::string buf;
	auto makeTerminal = [&] (std::string contents) -> PossibilityTerminal* {
		// For memory management
		_terminals.push_back(contents);
		return &_terminals.back();
	};

	while (*reading != 0) {
		if (*reading == '=') {
			auto foundB = buf.find_first_of('(');
			auto foundE = buf.find_last_of(')');
			if (foundB != std::string::npos && foundE != std::string::npos) {
				std::string name(buf.substr(0, foundB));
				current = &_nonterminals[name];
				std::string argsString = buf.substr(foundB + 1, foundE - foundB).c_str();
				const char* args = argsString.c_str();
				std::string argName;
				while (*args != 0) {
					if (*args == ',' || *args == ')') {
						_seedsFreeze[name].push_back(argName);
						argName.clear();
					} else if (*args == ':') {
						args++;
						std::string varName;
						while (*args != ',' && *args != ')') {
							varName.push_back(*args);
							args++;
						}
						std::hash<std::string> hash;
						_picksFreeze[name].push_back(std::pair<std::string, size_t>(argName, hash(varName)));
					} else argName.push_back(*args);
					args++;
				}
			} else current = &_nonterminals[buf];
			current->_possibilities.push_back(std::vector<Possibility*>());
			filled = &current->_possibilities.back();
			buf.clear();
		} else if (*reading == '\n') {
			filled->push_back(makeTerminal(buf));
			filled = nullptr;
			current = nullptr;
			buf.clear();
		} else if (*reading == '|') {
			if (!filled || !current) throw(std::runtime_error("CfgGenerator parsing error: misplaced | symbol"));
			filled->push_back(makeTerminal(buf));
			current->_possibilities.push_back(std::vector<Possibility*>());
			filled = &current->_possibilities.back();
			buf.clear();
		} else if (*reading == '\\' && reading[1] == 'n') {
			reading++;
			buf.push_back('\n');
		} else if (*reading == '\\' && reading[1] == 't') {
			reading++;
			buf.push_back('\t');
		} else {
			if (*reading == '{') {
				if (!filled) throw(std::runtime_error("CfgGenerator parsing error: misplaced { symbol"));
				filled->push_back(makeTerminal(buf));
				buf.clear();
			}
			if (*reading == '}') {
				if (!filled) throw(std::runtime_error("CfgGenerator parsing error: misplaced } symbol"));
				filled->push_back(makeTerminal(buf));
				buf.clear();
			} else buf.push_back(*reading);
		}
		reading++;
	}
	if (filled) filled->push_back(makeTerminal(buf));

	for (std::unordered_map<std::string, PossibilityNonterminal>::iterator it = _nonterminals.begin(); it != _nonterminals.end(); ++it) {
		PossibilityNonterminal& current = it->second;
		for (unsigned int i = 0; i < current._possibilities.size(); i++) {
			for (unsigned int j = 0; j < current._possibilities[i].size(); j++) {
				PossibilityTerminal* edited = static_cast<PossibilityTerminal*>(current._possibilities[i][j]);
				if (edited->_text[0] == '{') {
					std::string improved = edited->_text.substr(1);
					std::unordered_map<std::string, PossibilityNonterminal>::iterator found = _nonterminals.find(improved);
					if (found != _nonterminals.end()) {
						current._possibilities[i][j] = &found->second;
					} else {
						edited->_text[0] = '!';
					}
				}
			}
		}
	}
	for (const std::pair<const std::string, std::vector<std::string>>& it : _seedsFreeze) {
		PossibilityNonterminal& related = _nonterminals[it.first];
		for (const std::string& it2 : it.second) {
			related._seedsFreeze.push_back(&(_nonterminals[it2]));
		}
	}
	for (const std::pair<const std::string, std::vector<std::pair<std::string, size_t>>>& it : _picksFreeze) {
		PossibilityNonterminal& related = _nonterminals[it.first];
		for (const std::pair<std::string, size_t>& it2 : it.second) {
			related._picksFreeze.push_back(std::pair<Possibility*, size_t>(&(_nonterminals[it2.first]), it2.second));
		}
	}
}

void CfgGenerator::constructFromFile(std::string fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		throw(std::runtime_error("No such file"));
	}
	std::string input;

	file.seekg(0, std::ios::end);
	input.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	input.assign((std::istreambuf_iterator<char>(file)),
				 std::istreambuf_iterator<char>());

	file.close();

	constructFromString(input);
}

std::string CfgGenerator::generate(seedType offset) {
	if (_nonterminals.find("main") == _nonterminals.end()) {
		constructFromString("main=File is missing its main= nonterminal");
	}
	std::unordered_map<Possibility*, seedType> frozenSeeds;
	std::unordered_map<size_t, unsigned int> frozenPicks;
	std::unordered_map<Possibility*, size_t> frozenPickMap;
	return _nonterminals["main"].print(offset, frozenSeeds, frozenPicks, frozenPickMap);
}
