TEMPLATE = app
TARGET = heartServer

QT = core bluetooth
CONFIG    += console c++11

SOURCES = \
    main.cpp \
        wiringPi/wiringPi.c \
        wiringPi/wiringPiSPI.c \
    server.cpp \
    daqADS1298.cpp \
    daq.cpp \
    daqMPU6000.cpp \
    dataADS1298.cpp \
    dataMPU6000.cpp

HEADERS = \
    wiringPi/wiringPi.h \
    wiringPi/wiringPiSPI.h \
    server.h \
    daqADS1298.h \
    daq.h \
    daqMPU6000.h \
    dataADS1298.h \
    dataMPU6000.h

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/bluetooth
LIBS = -L/mnt/rasp-pi-rootfs/usr/lib -lwiringPi -lpthread -lbluetooth -lconfig

config.path = /home/pi/heartServer
config.files = /home/krakapwa/Documents/rpi2/heartServer/config/configADS1298.txt

target.path = /home/pi/heartServer
INSTALLS += target config

OTHER_FILES += config/configADS1298.txt
OTHER_FILES += config/configADS1298.txtbcgecg
