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

MATLABROOT=/home/krakapwa/bin/MATLAB/R2011b
INCLUDEPATH += /usr/include
LIBS += -I/usr/local/include -L/usr/local/lib -lwiringPi -L/mnt/rasp-pi-rootfs/usr/lib -lconfig

target.path = /home/pi/heartServer

EXTRA = \
        $${PWD}/config/configADS1298.txt \
        $${PWD}/config/configADS1298.txt.bcgecg \
        $${PWD}/exportPins \
        $${PWD}/syncUsb

for(FILE,EXTRA){
QMAKE_POST_LINK += $$quote(cp $${FILE} $${OUT_PWD}$$escape_expand(\n\t))
}

extra.files = $${EXTRA}
extra.path = /home/pi/heartServer

INSTALLS += target extra

DISTFILES += \
    exportPins
