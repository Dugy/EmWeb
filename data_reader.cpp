#include "data_reader.hpp"
#include <sstream>
#include <limits>
#include <algorithm>

constexpr double LOG_IDENTIFYING_THRESHOLD = 10;

GraphData readGraph(std::istream& file, const std::string& fileName) {

	std::string firstLine;
	std::getline(file, firstLine, '\n');
	char separator = ' ';
	for (char c : firstLine) {
		if (c == '\t') separator = '\t';
		else if (c == ';' && separator != '\t') separator = ';';
		else if (c == ',' && separator != '\t' && separator != ';') separator = ',';
	}

	unsigned int position = 0;
	auto readOne = [&] (const std::string& source) -> std::string {
		std::string assembled;
		while (source[position] != separator && source[position] != '\n' && source[position] != '\r' && source[position]) {
			assembled.push_back(source[position]);
			position++;
		}
		while (source[position] == separator || source[position] == '\r')
			position++;
		return assembled;
	};

	auto isDescription = [] (const std::string& maybe) {
		auto canBeNumber = [] (char letter) {
			if (letter >= '0' && letter <= '9') return true;
			if (letter == '-' || letter == 'e' || letter == 'E' || letter == '+' || letter == ' ' || letter == '.' || letter == ',') return true;
			return false;
		};
		if (maybe.empty()) return false;
		int decimalPoints = 0;
		for (char c : maybe) {
			if (!canBeNumber(c)) return true;
			if (c == '.' || c == ',') {
				decimalPoints++;
				if (decimalPoints > 1)
					return true;
			}
		}

		return false;
	};
	GraphData graph;
	graph.name = fileName;
	bool readTheX = false;
	while (firstLine[position] && firstLine[position] != '\n') {
		std::string part = readOne(firstLine);
		if (isDescription(part)) {
			graph.named = true;
		}
		if (readTheX)
			graph.lines.emplace_back();
		else
			readTheX = true;
	}

	position = 0;
	if (graph.named) {
		graph.xName = readOne(firstLine);
		while (firstLine[position] && firstLine[position] != '\n') {
			graph.lines.back().name = readOne(firstLine);
		}
	}

	auto readNumber = [] (const std::string& asString) {
		if (asString.empty() || (asString[0] >= 'a' && asString[0] <= 'z') || (asString[0] >= 'A' && asString[0] <= 'Z'))
			return std::numeric_limits<double>::quiet_NaN();

		std::stringstream sstream(asString);
		double number;
		sstream >> number;
		return number;
	};

	std::vector<double> numbers;
	double sum = 0;
	auto addLine = [&] (const std::string& line) {
		position = 0;
		graph.valuesX.push_back(readNumber(readOne(line)));
		for (GraphLine& series : graph.lines) {
			double got = readNumber(readOne(line));
			series.valuesY.push_back(got);
			if (got == got) { // If a valid number
				numbers.push_back(got);
				sum += got;
				series.validPoints++;
			}
		}
	};

	std::string line;
	if (!graph.named) {
		addLine(firstLine);
	}
	while (std::getline(file, line, '\n')) {
		addLine(line);
	}

	if (!numbers.empty()) {
		std::sort(numbers.begin(), numbers.end());
		double average = sum / numbers.size();
		double median = numbers[numbers.size() / 2];
		if (average > median * LOG_IDENTIFYING_THRESHOLD)
			graph.logarithmic = true;

		for (auto& it : graph.lines)
			graph.maxValidPoints = std::max(graph.maxValidPoints, it.validPoints);
	}
	return graph;
}
