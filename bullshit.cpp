#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "bullshit.hpp"
#include "ui_bullshit.h"
#include "environment.h"
#include "serialisable/serialisable_brief.hpp"
#include "contextfree_generator/cfg_generator.hpp"



#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QEventLoop>
#include <QFileInfo>

Bullshit::Bullshit() :
	ui(new Ui::Bullshit)
{
	ui->setupUi(this);
}

void Bullshit::load() {
	auto index = getFile("bullshits.json");
	struct : SerialisableBrief {
		struct BullshitEntry : SerialisableBrief {
			std::string titles = key("titles");
			std::string contents = key("contents");
			std::string description = key("description");
			std::string language = key("language");
		};
		std::unordered_map<std::string, BullshitEntry> bullshits = key("bullshits");
	} bullshits;
	bullshits.deserialise(index);

	QLayoutItem* item;
	while((item = ui->verticalLayout->takeAt(0))) {
		if (item->layout()) {
			delete item->layout();
		}
		if (item->widget()) {
		   delete item->widget();
		}
		delete item;
	}

	if (_contentIndentifier.empty()) {
		for (auto& it : bullshits.bullshits) {
			if ((_navigator->language() == Language::ENGLISH && it.second.language == "en")
					|| (_navigator->language() == Language::SLOVAK && it.second.language == "sk")) {
				QPushButton* button = new QPushButton(QString::fromStdString(it.second.description), this);
				ui->verticalLayout->addWidget(button);
				button->connect(button, &QPushButton::clicked, [this, name = it.first] {
					_contentIndentifier = name;
					load();
				});
			}
		}

		ui->verticalLayout->addStretch(1);
		QPushButton* button = new QPushButton(QString::fromStdString("Change language to view more"), this);
		ui->verticalLayout->addWidget(button);
		button->connect(button, &QPushButton::clicked, [this] {
			_navigator->language((_navigator->language() == Language::ENGLISH) ? Language::SLOVAK : Language::ENGLISH);
			load();
		});
	} else {
		constexpr int PARTS = 2;
		std::array<std::string, PARTS> fileNames = {bullshits.bullshits.at(_contentIndentifier).titles,
												bullshits.bullshits.at(_contentIndentifier).contents };
		std::array<std::string, PARTS> generated;
		for (int i = 0; i < PARTS; i++) {
			auto fileContents = getFile(fileNames[i]);
			CfgGenerator generator;
			generator.constructFromString(fileContents);
			generated[i] = generator.generate(rand());
		}

		QLabel* title = new QLabel(QString::fromStdString(generated[0]), this);
		QFont font = title->font();
		font.setPointSize(18);
		title->setFont(font);
		ui->verticalLayout->addWidget(title);
		QTextEdit* contents = new QTextEdit(this);
		contents->setMarkdown(QString::fromStdString(generated[1]));
		contents->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse
										  | Qt::TextInteractionFlag::LinksAccessibleByMouse);
		ui->verticalLayout->addWidget(contents, 1);
	}
	_navigator->updateSettings();
}

Bullshit::~Bullshit()
{
	delete ui;
}
