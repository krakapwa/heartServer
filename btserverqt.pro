TEMPLATE = app
TARGET = btserverqt

QT = core
#CONFIG    += console
#include(../../src/qtsinglecoreapplication.pri)

SOURCES = \
    main.cpp \
        wiringPi/wiringPi.c \
        wiringPi/wiringPiSPI.c \
    server.cpp \
    daq.cpp

HEADERS = \
        wiringPi/wiringPi.h \
        wiringPi/wiringPiSPI.h \
    server.h \
    daq.h

target.path = /home/rpi/btserverqtbuild
conf.path = $$target.path
conf.files = $$_PRO_FILE_PWD_/config.txt
INSTALLS += target conf

QMAKE_CFLAGS = -I/usr/lib
QMAKE_CXXFLAGS = -I/usr/lib
INCLUDEPATH += /usr/include
#LIBS += -L/usr/lib -lwiringPi -lbluetooth
LIBS += -L/mnt/rasp-pi-rootfs/usr/lib -lwiringPi -lpthread -lbluetooth -lconfig
