#include <daq.h>

uint8_t tmp2[27];

int Daq::getChan(){return chan;}

Daq::Daq(QObject  *parent)
        : QObject(parent)
{
    rootPath = "/home/pi/heartServer/";
    quit = false;
}

void Daq::setCfgFileName(QString arg_cfgFileName){
    cfgFileName = rootPath + arg_cfgFileName;
    qDebug() << "Setting config file name";
    qDebug() << cfgFileName;
}

void Daq::setServ(Server& servIn){

    serv  = &servIn;
}

Daq::~Daq()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}


void Daq::loadCfg(){

    //Read config file
    qDebug() << "Initializing config file" ;
    qDebug() << cfgFileName;
    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, cfgFileName.toUtf8().constData()))
    {
        qDebug() << config_error_file(&cfg) << " " <<  config_error_line(&cfg) << " " <<config_error_text(&cfg);
        config_destroy(&cfg);
    }

    qDebug() << "test";
    /*Read the parameter group*/
    //setting = config_lookup(&cfg, "registers");

}
