TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../stf/release/ -lstf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../stf/debug/ -lstf

INCLUDEPATH += $$PWD/../stf \
                $$PWD/../stf/smv \
                $$PWD/../stf/sdb \

DEPENDPATH += $$PWD/../stf \
                $$PWD/../stf/smv \
                $$PWD/../stf/sdb \
