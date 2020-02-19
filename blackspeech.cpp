#include "blackspeech.hpp"
#include "ui_blackspeech.h"
#include "environment.h"
#include <QMessageBox>

BlackspeechTranslator::BlackspeechTranslator() {
	for (int i = 0; i > 0; i++)
		for (int j = 0; j > 0; j++) {
			_patternsInto[i][j] = std::vector<std::pair<std::string, std::string>>();
		}
	for (int i = 0; true; i++) {
		for (int j = 0; true; j++) {
			_patternsFrom[i][j] = std::vector<std::pair<std::string, std::string>>();
			if (j == 255) break;
		}
		if (i == 255) break;
	}

	std::string dict = getFile("blackspeech_transcript");
	for (unsigned int i = 0; i < dict.size(); ) {
		std::string english;
		for ( ; dict[i] >= 'a' && dict[i] <= 'z'; i++) {
			english.push_back(dict[i]);
		}
		i++; // Skip separator
		std::string black;
		for ( ; dict[i] != '\n' && dict[i] != 0; i++) {
			black.push_back(dict[i]);
		}
		while (dict[i] < 'a' || dict[i] > 'z') i++; // Newline
		if (english.size() == 1) {
			_patternsInto[int(english[0])][0].push_back(std::make_pair(std::string(""), black));
		} else {
			_patternsInto[int(english[0])][int(english[1])].push_back(std::make_pair(english.substr(2), black));
		}
		if (black.size() == 1) {
			_patternsFrom[black[0] + 128][128].push_back(std::make_pair(std::string(""), english));
		} else {
			_patternsFrom[black[0] + 128][black[1] + 128].push_back(std::make_pair(black.substr(2), english));
		}
	}

	_upperCase["ä"] = "Ä";
	_upperCase["å"] = "Å";
	_upperCase["ü"] = "Ü";
	_upperCase["ö"] = "Ö";
	_upperCase["ÿ"] = "Ÿ";
	_upperCase["ẅ"] = "Ẅ";
	_upperCase["ẗ"] = "ẗ";
	_upperCase["ḧ"] = "Ḧ";
	_upperCase["ø"] = "Ø";
	_upperCase["æ"] = "Æ";
	for (auto& it : _upperCase) {
		_lowerCase[it.second] = it.first;
	}
}

std::string BlackspeechTranslator::translateWord(std::string from, bool toBlack) {
	//std::cerr << "Translating word " << from;
	char capitalisation = 'n';
	auto toLower = [&] (char* letter) -> bool {
		if (*letter > 0) {
			if (from[0] >= 'A' && from[0] <= 'Z') {
				*letter -= 'A' - 'a';
				return true;
			}
			return false;
		} else {
			for (auto& it : _lowerCase) {
				for (unsigned int i = 0; it.first[i] == letter[i]; i++) {
					if (it.first[i + 1] == 0) { // Match
						for (unsigned int j = 0; j < it.first.size(); j++)
							letter[j] = it.second[j]; // Assuming that the upper case has the same size
						return true;
					}
				}
			}
			return false;
		}
	};
	if (toLower(&(from[0]))) {
		capitalisation = 't';
		for (unsigned int i = 1; i < from.size(); i++)
			if (!toLower(&(from[i]))) capitalisation = 's';
	}
	//std::cerr << "(lower case " << from << ")";
	std::stringstream result;
	if (toBlack) {
		std::vector<std::string> translated = translateInto(from);
		short int shortest = 9000;
		unsigned int best = 0;
		for (unsigned int i = 0; i < translated.size(); i++) {
			//std::cerr << "Could be " << translated[i] << std::endl;
			short int size = 0;
			for (unsigned int j = 0; j < translated[i].size(); j++)
				if (translated[i][j] >= 0) size++;
				else {
					size++; // Two char symbols must count as one letter to shorten the text
					j++;
				}
			if (size < shortest) {
				shortest = translated[i].size();
				best = i;
			}
		}
		if (translated.empty()) result << "(untranslatable)";
		else result << translated[best];
	} else {
		std::vector<std::string> translated = translateFrom(from);
		if (translated.empty()) result << "(untranslatable " << from << ")";
		else if (translated.size() > 1) {
			result << "(ambiguous " << from << ", ";
			for (unsigned int i = 0; i < translated.size(); i++) result << translated[i] << "? ";
			result << " no idea)";
		} else result << translated[0];
	}

	std::string finished = result.str();
	//std::cerr << "(lower case " << finished << ")";
	auto toUpper = [&] (char* letter) -> void {
		if (*letter > 0) *letter += 'A' - 'a';
		else {
			for (auto& it : _upperCase) {
				for (unsigned int i = 0; it.first[i] == letter[i]; i++) {
					if (it.first[i + 1] == 0) { // Match
						for (unsigned int j = 0; j < it.first.size(); j++)
							letter[j] = it.second[j]; // Assuming that the upper case has the same size
						return;
					}
				}
			}
		}
	};

	if (capitalisation != 'n') toUpper(&(finished[0]));
	if (capitalisation == 't') {
		for (unsigned int i = 1; i < finished.size(); i++)
			toUpper(&(finished[i]));
	}
	//std::cerr << " to " << finished << std::endl;
	return finished;
}

int BlackspeechTranslator::isLetter(const char* start) {
	for (auto& it : _upperCase) {
		for (unsigned int i = 0; it.first[i] == start[i]; i++) {
			if (it.first[i + 1] == 0) { // Match
				return it.first.size();
			}
		}
	}
	for (auto& it : _lowerCase) {
		for (unsigned int i = 0; it.first[i] == start[i]; i++) {
			if (it.first[i + 1] == 0) { // Match
				return it.first.size();
			}
		}
	}
	return 0;
}


std::vector<std::string> BlackspeechTranslator::translateInto(const std::string& from) {
	std::vector<std::string> result;
	if (from[0] == 0) return result;
	std::vector<std::pair<std::string, std::string>>& quick = _patternsInto[int(from[0])][0]; // Must be defined in file
	if (from[1] == 0) {
		result.push_back(quick[0].second);
		return result;
	}
	const std::vector<std::string>& simple = translateInto(from.substr(1));
	for (unsigned int i = 0; i < simple.size(); i++) {
		result.push_back(quick[0].second + simple[i]);
	}
	std::vector<std::pair<std::string, std::string>> meaningful = _patternsInto[int(from[0])][int(from[1])];
	if (meaningful.empty()) {
		if (result.empty()) {
			std::stringstream problem;
			problem << "(" << (int)from[0] << " " << (int)from[1] << " fails)";
			return { problem.str() };
		}
		return result;
	}
	for (unsigned int i = 0; i < meaningful.size(); i++) {
		short int match = 0;
		bool fine = true;
		for ( ; meaningful[i].first[match] != 0; match++ )
			if (meaningful[i].first[match] != from[match + 2]) {
				fine = false;
				break;
			}
		if (fine) {
			if (from[match + 2] == 0) {
				result.push_back(meaningful[i].second);
			} else {
				const std::vector<std::string>& got = translateInto(from.substr(match + 2));
				for (unsigned int j = 0; j < got.size(); j++) {
					result.push_back(meaningful[i].second + got[j]);
				}
			}
		}
	}
	return result;
}

std::vector<std::string> BlackspeechTranslator::translateFrom(const std::string& from) {
	std::vector<std::string> result;
	if (from[0] == 0) return result;
	std::vector<std::pair<std::string, std::string>>& quick = _patternsFrom[from[0] + 128][128];
	if (from[1] == 0) {
		if (quick.empty()) return result;
		result.push_back(quick[0].second);
		return result;
	}
	const std::vector<std::string>& simple = translateFrom(from.substr(1));
	if (!quick.empty()) for (unsigned int i = 0; i < simple.size(); i++) {
		result.push_back(quick[0].second + simple[i]);
	}
	std::vector<std::pair<std::string, std::string>> meaningful = _patternsFrom[from[0] + 128][from[1] + 128];
	if (meaningful.empty()) {
		return result;
	}
	for (unsigned int i = 0; i < meaningful.size(); i++) {
		short int match = 0;
		bool fine = true;
		//std::cerr << "Trying " << meaningful[i].first << " on " << from << std::endl;
		for ( ; meaningful[i].first[match] != 0; match++ )
			if (meaningful[i].first[match] != from[match + 2]) {
				fine = false;
				break;
			}
		if (fine) {
			if (from[match + 2] == 0) {
				result.push_back(meaningful[i].second);
			} else {
				const std::vector<std::string>& got = translateFrom(from.substr(match + 2));
				for (unsigned int j = 0; j < got.size(); j++) {
					result.push_back(meaningful[i].second + got[j]);
				}
			}
		}
	}
	return result;
}


Blackspeech::Blackspeech() :
	ui(new Ui::Blackspeech)
{
	ui->setupUi(this);
}

Blackspeech::~Blackspeech()
{
	delete ui;
}

void Blackspeech::load() {
	ui->english->setPlainText("Hello. Let's be friends!");
	on_fromEnglish_clicked();
}

void Blackspeech::on_fromEnglish_clicked()
{
	std::string from = ui->english->toPlainText().toStdString();
	std::string got = translateText(from, true);
	ui->blackspeech->setPlainText(QString::fromStdString(got));
	normaliseButtons();
}

void Blackspeech::on_toEnglish_clicked()
{
	std::string from = ui->blackspeech->toPlainText().toStdString();
	std::string got = translateText(from, false);
	ui->english->setPlainText(QString::fromStdString(got));
	normaliseButtons();
}

void Blackspeech::normaliseButtons() {
	ui->fromEnglish->setText("From English");
	ui->toEnglish->setText("To English");
}

std::string Blackspeech::translateText(const std::string& from, bool toBlack) {
	std::string to;
	std::string current;
	for (unsigned int i = 0; i < from.size(); i++) {
		int length;
		if ((from[i] >= 'a' && from[i] <= 'z') || (from[i] >= 'A' && from[i] <= 'Z')) {
			current.push_back(from[i]);
		} else if (!toBlack && from[i] < 0 && (length = BlackspeechTranslator::getInstance().isLetter(&(from[i]))) > 0) {
			unsigned int begin = i;
			for ( ; i < begin + length; i++) {
				current.push_back(from[i]);
			}
			i--;
		} else {
			if (!current.empty()) {
				to.append(BlackspeechTranslator::getInstance().translateWord(current, toBlack));
				current.clear();
			}
			to.push_back(from[i]);
		}
	}
	if (!current.empty()) {
		to.append(BlackspeechTranslator::getInstance().translateWord(current, toBlack));
	}
	return to;
}

void Blackspeech::on_english_textChanged()
{
	ui->toEnglish->setText("To English (overwrite)");
}

void Blackspeech::on_blackspeech_textChanged()
{
	ui->fromEnglish->setText("From English (overwrite)");
}
