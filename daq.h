#ifndef DAQ_H
#define DAQ_H
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <string.h>
#include <libconfig.h>
#include <QDebug>
#include <QThread>
#include <wiringPi/wiringPi.h>
#include <wiringPi/wiringPiSPI.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <data.h>
#include <fstream>
#include <iostream>

class Server;


class Daq: public QObject
{
    Q_OBJECT

public:
    Daq(QObject *parent = 0);
    ~Daq();
    void virtual setup()=0;
    void setCfgFileName(QString);
    std::ofstream myFile;
    int getChan();

private:

    void setServ(Server&);

protected:
    int chan;
    void run();
    QString rootPath;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    Server* serv;
    config_t cfg;
    config_setting_t *setting;
    QString cfgFileName;
    void loadCfg();

};

#endif // DAQ_H
