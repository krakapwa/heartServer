#include "daq.h"

Daq::Daq(QObject  *parent)
        : QObject(parent)
{
    rootPath = "/home/pi/btserverqtbuild/";
    quit = false;
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

void Daq::loadCfg(std::string cfgFileName){

    //Read config file
    qDebug() << "Initializing config file";
    config_init(&cfg);

    config_file_name = rootPath + cfgFileName;

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, config_file_name.c_str()))
    {
        qDebug() << config_error_file(&cfg) << " " <<  config_error_line(&cfg) << " " <<config_error_text(&cfg);
        config_destroy(&cfg);
    }

    /*Read the parameter group*/
    setting = config_lookup(&cfg, "registers");

}
