#ifndef BLACKSPEECH_HPP
#define BLACKSPEECH_HPP

#include "content_base.hpp"

class BlackspeechTranslator
{
public:
	static BlackspeechTranslator& getInstance()
	{
		static BlackspeechTranslator instance;
		return instance;
	}

	std::string translateWord(std::string from, bool toBlack);

	int isLetter(const char* start);

private:
	BlackspeechTranslator();

	std::vector<std::pair<std::string, std::string>> _patternsFrom[256][256];
	std::vector<std::pair<std::string, std::string>> _patternsInto[128][128];
	std::map<std::string, std::string> _upperCase;
	std::map<std::string, std::string> _lowerCase;

	std::vector<std::string> translateInto(const std::string& from);
	std::vector<std::string> translateFrom(const std::string& from);

	BlackspeechTranslator(BlackspeechTranslator const&) = delete;
	void operator=(BlackspeechTranslator const&) = delete;
};

namespace Ui {
class Blackspeech;
}

class Blackspeech : public ContentBase
{
	Q_OBJECT

public:
	explicit Blackspeech();
	~Blackspeech();

	void serialisation() override {
		ContentBase::serialisation();
		subclass("evil");
	}

	void load() override;

	std::string translateText(const std::string& from, bool toBlack);

private slots:
	void on_fromEnglish_clicked();

	void on_toEnglish_clicked();

	void on_english_textChanged();

	void on_blackspeech_textChanged();

private:
	Ui::Blackspeech *ui;
	SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, Blackspeech, "evil");

	void normaliseButtons();
};

#endif // BLACKSPEECH_HPP
