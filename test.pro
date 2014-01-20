TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    localtcpserver.cpp \
    localtcpclient.cpp

HEADERS += \
    mysock.h

