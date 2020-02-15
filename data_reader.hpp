#ifndef DATA_READER_HPP
#define DATA_READER_HPP
#include <vector>
#include <string>
#include <istream>

struct GraphLine {
	std::string name;
	std::vector<double> valuesY;
	int validPoints = 0;
};

struct GraphData {
	std::string name;
	std::string xName;
	std::vector<double> valuesX;
	std::vector<GraphLine> lines;
	bool named = false;
	bool logarithmic = false;
	int maxValidPoints = 0;

};

GraphData readGraph(std::istream& file, const std::string& fileName);

#endif // DATA_READER_HPP
