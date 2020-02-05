#-------------------------------------------------
#
# Project created by QtCreator 2020-01-03T00:11:20
#
#-------------------------------------------------

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = emweb
TEMPLATE = app

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000	# disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
		bullshit.cpp \
		components_list.cpp \
		content_base.cpp \
		default_screen.cpp \
		main.cpp \
		navigator.cpp

HEADERS += \
		bullshit.hpp \
		components_list.h \
		content_base.hpp \
		default_screen.h \
		environment.h \
		i_navigator.h \
		navigator.h

FORMS += \
		bullshit.ui \
		components_list.ui \
		default_screen.ui \
		navigator.ui
