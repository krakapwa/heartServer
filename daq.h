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
#include <fstream>

const std::string serverBtAdr = "00:02:72:C9:1B:25";

class Server;
class Data;

class Daq: public QObject
{
    Q_OBJECT

public:
    Daq(QObject *parent = 0);
    ~Daq();
    void virtual setup()=0;
    void run();

private:

    void setServ(Server&);

protected:

    std::string rootPath;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    Server* serv;

    //Config
    config_t cfg;
    config_setting_t *setting;
    std::string config_file_name;
    void loadCfg(std::string);

};

#endif // DAQ_H
