#include "daq.h"
#include "server.h"

// Variables used in static function
std::fstream myFile;
DataOut y;
DataOut buffer;

Daq::Daq(QObject  *parent)
    :QObject(parent), quit(false)
{
    rootPath = "/home/rpi/btserverqtbuild/";
    ads1298setup();
    quit = false;
}

Daq::~Daq()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void Daq::getData(void){

    uint8_t spiData[numSerialBytes] = {0};

    wiringPiSPIDataRW(chan, spiData ,numSerialBytes);

    for(int i=0;i<numSerialBytes; ++i){
       y.spiData[i]	= spiData[i];
    }

    appendToFile(y);
}

void Daq::setServ(Server& servIn){

   serv  = &servIn;
}

void Daq::startContinuous(QString fname){
    std::string fnamePath;
    fnamePath = rootPath + fname.toStdString();

    myFile.open(fnamePath.c_str(), std::ios::out|std::ios::binary);
    sendCmd(RDATAC);
    wiringPiISR(DRDY, INT_EDGE_FALLING, &Daq::getData);
    digitalWrite(START,HIGH);
}

void Daq::stopContinuous(){

    digitalWrite(START,LOW);
    //myFile.fsync();
    //fsync(myFile);
    myFile.close();
    //myFile.open(fnamePath.c_str(), std::ios::out|std::ios::binary);
}

void Daq::writeReg(uint8_t address, uint8_t data)
{

    // SDATAC (stop read data continuous mode) default mode
    uint8_t spiDataCmd[2];
    spiDataCmd[0] = SDATAC;
    wiringPiSPIDataRW(chan, spiDataCmd ,1);
    delay(10);

    uint8_t spiDataWrite[3];
    spiDataWrite[0] = WREG+address;
    spiDataWrite[1] = 0x00;
    spiDataWrite[2] = data;
    wiringPiSPIDataRW(chan, spiDataWrite, 3);
}

void Daq::sendCmd(uint8_t cmd)
{

    uint8_t spiDataCmd[1];
    spiDataCmd[0]=cmd;
    wiringPiSPIDataRW(chan, spiDataCmd ,1);
    delay(10);
}

uint8_t Daq::readReg(uint8_t address)
{
    sendCmd(SDATAC);

    uint8_t spiDataRead[3];
    spiDataRead[0] = RREG+address;
    spiDataRead[1] = 0x00;
    spiDataRead[2] = 0x00;
    wiringPiSPIDataRW(chan, spiDataRead, 3);
    delay(10);

    return spiDataRead[2];
}

void Daq::ads1298setup ()
{
    //serv->printWriteLog("Starting ADS1298 setup...") ;
    qDebug() << "Starting ADS1298 setup...";
    int fclk = 7000000; //2.048MHz (max 20MHz)

    qDebug() <<  "Calling wiringPiSetup()";
    wiringPiSetup(); //init SPI pins

    int fd;
    qDebug() << "Calling wiringPiSetup()";
    fd=wiringPiSPISetup (chan, fclk); //init SPI pins

    uint8_t spiMode = SPI_MODE_1;
    qDebug() << "Setting SPI mode to 1";
    // Change to SPI mode 1 on write (default is 0)
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);

    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    pinMode(START, OUTPUT); //START
    pinMode(DRDY, INPUT); //DRDY
    pinMode(nRESET, OUTPUT); //_RESET
    pinMode(CLKSEL, OUTPUT);
    pinMode(nCS, OUTPUT);
    pullUpDnControl (nCS, PUD_DOWN) ;
    pullUpDnControl (MOSI, PUD_OFF) ;
    pullUpDnControl (MISO, PUD_OFF) ;


    // Power-up sequence
    qDebug() << "Power-up sequence";
    digitalWrite(CLKSEL, HIGH);
    delay(100);
    digitalWrite(nRESET,HIGH);
    delay(1000);
    digitalWrite(nRESET,LOW);
    delay(100);
    digitalWrite(nRESET,HIGH);

    // SDATAC (stop read data continuous mode) default mode
    qDebug() << "Sending SDATAC";
    sendCmd(SDATAC);


    //Read config file
    int cfgtmp;
    int len = 27;
    uint8_t spiDataWrite[len];
    config_init(&cfg);

    //config_file_name = "/home/rpi/btserverqtbuild/config.txt";

    config_file_name = rootPath + "config.txt";

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, config_file_name.c_str()))
    {
        qDebug() << config_error_file(&cfg) << " " <<  config_error_line(&cfg) << " " <<config_error_text(&cfg);
        config_destroy(&cfg);
    }

    /*Read the parameter group*/
    qDebug() << "test";
    setting = config_lookup(&cfg, "registers");


    spiDataWrite[0] = WREG+1; // WREG +1 (address of CONFIG1)
    spiDataWrite[1] = 25 - 1; // Num of regs to write - 1

    if (setting != NULL)
    {
        /*Read the string*/
        config_setting_lookup_int(setting, "CONFIG1", &cfgtmp);
        spiDataWrite[2] = cfgtmp;
        config_setting_lookup_int(setting, "CONFIG2", &cfgtmp);
        spiDataWrite[3] = cfgtmp;
        config_setting_lookup_int(setting, "CONFIG3", &cfgtmp);
        spiDataWrite[4] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF", &cfgtmp);
        spiDataWrite[5] = cfgtmp;
        config_setting_lookup_int(setting, "CH1set", &cfgtmp);
        spiDataWrite[6] = cfgtmp;
        config_setting_lookup_int(setting, "CH2set", &cfgtmp);
        spiDataWrite[7] = cfgtmp;
        config_setting_lookup_int(setting, "CH3set", &cfgtmp);
        spiDataWrite[8] = cfgtmp;
        config_setting_lookup_int(setting, "CH4set", &cfgtmp);
        spiDataWrite[9] = cfgtmp;
        config_setting_lookup_int(setting, "CH5set", &cfgtmp);
        spiDataWrite[10] = cfgtmp;
        config_setting_lookup_int(setting, "CH6set", &cfgtmp);
        spiDataWrite[11] = cfgtmp;
        config_setting_lookup_int(setting, "CH7set", &cfgtmp);
        spiDataWrite[12] = cfgtmp;
        config_setting_lookup_int(setting, "CH8set", &cfgtmp);
        spiDataWrite[13] = cfgtmp;
        config_setting_lookup_int(setting, "RLD_SENSP", &cfgtmp);
        spiDataWrite[14] = cfgtmp;
        config_setting_lookup_int(setting, "RLD_SENSN", &cfgtmp);
        spiDataWrite[15] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF_SENSP", &cfgtmp);
        spiDataWrite[16] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF_SENSN", &cfgtmp);
        spiDataWrite[17] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF_FLIP", &cfgtmp);
        spiDataWrite[18] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF_STATP", &cfgtmp);
        spiDataWrite[19] = cfgtmp;
        config_setting_lookup_int(setting, "LOFF_STATN", &cfgtmp);
        spiDataWrite[20] = cfgtmp;
        config_setting_lookup_int(setting, "GPIO", &cfgtmp);
        spiDataWrite[21] = cfgtmp;
        config_setting_lookup_int(setting, "PACE", &cfgtmp);
        spiDataWrite[22] = cfgtmp;
        config_setting_lookup_int(setting, "RESP", &cfgtmp);
        spiDataWrite[23] = cfgtmp;
        config_setting_lookup_int(setting, "CONFIG4", &cfgtmp);
        spiDataWrite[24] = cfgtmp;
        config_setting_lookup_int(setting, "WCT1", &cfgtmp);
        spiDataWrite[25] = cfgtmp;
        config_setting_lookup_int(setting, "WCT2", &cfgtmp);
        spiDataWrite[26] = cfgtmp;

    }

    config_destroy(&cfg);

    //Write registers
    qDebug() << "Writing config registers";
    wiringPiSPIDataRW(chan, spiDataWrite, len);
    delay(100);

    qDebug() << "Reading config registers";
    printRegs();
    delay(100);

    qDebug() << "ADS1298 setup done.";
}

void Daq::appendToFile(DataOut y){

    myFile.write((char*)&y, sizeof (DataOut));

}


void Daq::writeToBuffer(DataOut &y){
    buffer = y;
}

void Daq::printRegs(){

    qDebug() << "Printing values of registers";
    uint8_t res;
    int len = 26;

    for(int i=0; i<len;++i){

        res = readReg((uint8_t)i);
        qDebug() << "Register: " + QString::number(i) + " = " + QString::number(res);
    }
}
