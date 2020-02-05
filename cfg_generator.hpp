#ifndef CFG_GENERATOR_H
#define CFG_GENERATOR_H

#include <unordered_map>
#include <vector>
#include <string>
#include <list>
#include <climits>

#include <iostream>

class CfgGenerator
{
private:
	using seedType = int;
	static short int nextRand(int& seed);
	
	struct Possibility {
		virtual std::string print(seedType& seed, std::unordered_map<Possibility*, seedType>& frozenSeeds,
				std::unordered_map<size_t, unsigned int>& frozenPicks, std::unordered_map<Possibility*, size_t>& frozenPickMap) = 0;
	};

	struct PossibilityTerminal : public Possibility {
		PossibilityTerminal(std::string& from) : _text(from) {}
		virtual std::string print(seedType& seed, std::unordered_map<Possibility*, seedType>& frozenSeeds,
				std::unordered_map<size_t, unsigned int>& frozenPicks, std::unordered_map<Possibility*, size_t>& frozenPickMap);
		std::string _text;
	};

	struct PossibilityNonterminal : public Possibility {
		PossibilityNonterminal() : last(1) {}
		virtual std::string print(seedType& seed, std::unordered_map<Possibility*, seedType>& frozenSeeds,
								  std::unordered_map<size_t, unsigned int>& frozenPicks, std::unordered_map<Possibility*, size_t>& frozenPickMap);
		std::vector<std::vector<Possibility*> > _possibilities;
		std::vector<Possibility*> _seedsFreeze;
		std::vector<std::pair<Possibility*, size_t>> _picksFreeze;
		unsigned int last;
		seedType lastSeed; // Does not need to be initialised
	};
	friend struct Possibility;
	friend struct PossibilityTerminal;
	friend struct PossibilityNonterminal;

	std::unordered_map<std::string, PossibilityNonterminal> _nonterminals;
	std::list<PossibilityTerminal> _terminals;
public:
	CfgGenerator();
	void constructFromString(std::string code);
	void constructFromFile(std::string fileName);
	std::string generate(seedType offset = 0);
	~CfgGenerator();
};

#endif // CFG_GENERATOR_H
