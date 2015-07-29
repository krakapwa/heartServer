#ifndef DAQ_H
#define DAQ_H
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <string.h>
#include <libconfig.h>
#include <QDebug>
#include <QThread>
#include "wiringPi/wiringPi.h"
#include "wiringPi/wiringPiSPI.h"
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <data.h>
#include <fstream>
#include <iostream>
#include "server.h"

class Daq: public QObject
{
    Q_OBJECT

public:
    Daq(QObject *parent = 0);
    ~Daq();
    virtual void setup()=0;
    virtual std::vector<int32_t>* getData()=0;
    void setCfgFileName(QString);
    int getChan();
    int getFs();
    virtual int getNbytes() = 0;
    virtual int getNchans() = 0;

protected:
    int chan;
    void run();
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    config_t cfg;
    config_setting_t *setting;
    QString cfgFileName;
    void loadCfg();
    int fs;

};

#endif // DAQ_H
