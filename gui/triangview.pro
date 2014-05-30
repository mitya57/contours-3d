CONFIG += warn_all
CONFIG += debug

QT += opengl

SOURCES += triangview.cpp main.cpp
SOURCES += ../src/triang.cpp ../src/distarray.cpp
HEADERS += triangview.h
INCLUDEPATH += ../src
