QT += widgets

TEMPLATE = lib
#测试用
#TEMPLATE = app

CONFIG += c++11

#测试用
equals(TEMPLATE,app){
    SOURCES += main.cpp
}

OTHER_FILES += \
    README.md

target.path = /usr/lib
INSTALLS +=  target


target_examples.path = $$DESTDIR/usr/share/kiranwidgets-qt5/examples/
target_includes.path = $$DESTDIR/usr/include/kiranwidgets-qt5/
INSTALLS += target_examples target_includes
include(kiran-titlebar-window/kiran-titlebar-window.pri)
