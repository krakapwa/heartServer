TEMPLATE = app
TARGET = btserverqt

QT = core widgets
CONFIG    += console

SOURCES = \
    main.cpp \
        wiringPi/wiringPi.c \
        wiringPi/wiringPiSPI.c \
    server.cpp \
    daqADS1298.cpp \
    daq.cpp

HEADERS = \
        wiringPi/wiringPi.h \
        wiringPi/wiringPiSPI.h \
    server.h \
    daqADS1298.h \
    daq.h \
    data.h

target.path = /home/pi/btserverqtbuild
conf.path = $$target.path
conf.files = $$_PRO_FILE_PWD_/configADS1298.txt
INSTALLS += target conf

INCLUDEPATH += /usr/include/bluetooth
INCLUDEPATH += /usr/include
LIBS = -L/mnt/rasp-pi-rootfs/usr/lib -lwiringPi -lpthread -lbluetooth -lconfig
