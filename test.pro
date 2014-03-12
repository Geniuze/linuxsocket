TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    localtcpserver.cpp \
    localtcpclient.cpp \
    udpserver.cpp \
    udpclient.cpp

HEADERS += \
    mysock.h

