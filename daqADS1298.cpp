#include "daqADS1298.h"

static uint8_t tmp[nSerialBytes];

DaqADS1298::DaqADS1298(){
    DRDY = ADS1298_DRDY;
    //tmpSpiData[nSerialBytes] = {0};
}

void DaqADS1298::getData(){

    //uint8_t tmp[nSerialBytes];
    digitalWrite(ADS1298_nCS,LOW);
    wiringPiSPIDataRW(ADS1298_chan, tmp ,nSerialBytes);
    digitalWrite(ADS1298_nCS,HIGH);

    //qDebug() << tmp[10];
    Daq::getWriteData(&myFile,tmp, nSerialBytes);
}

void DaqADS1298::startContinuous(QString fname){
    QString fnamePath;
    fnamePath = rootPath + fname;
    qDebug() << "opening file " + fnamePath;
    myFile.open(fnamePath.toStdString(), std::ios::out | std::ios::binary);
    //myFile.setFileName(fnamePath);
    //myFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    //myFile.open(QIODevice::WriteOnly);
    //outStream.setDevice(&myFile);

    digitalWrite(ADS1298_START,HIGH);
    delay(10);
    sendCmd(ADS1298_RDATAC);
}

void DaqADS1298::stopContinuous(){

    digitalWrite(ADS1298_START,LOW);
    delay(1);
    qDebug() << "Closing file";
    myFile.close();
}

void DaqADS1298::writeReg(uint8_t address, uint8_t data)
{

    // ADS1298_SDATAC (stop read data continuous mode) default mode
    uint8_t spiDataCmd[2];
    spiDataCmd[0] = ADS1298_SDATAC;

    digitalWrite(ADS1298_nCS,LOW);

    //delayMicroseconds(1);
    wiringPiSPIDataRW(ADS1298_chan, spiDataCmd ,1);

    uint8_t spiDataWrite[3];
    spiDataWrite[0] = ADS1298_WREG+address;
    spiDataWrite[1] = 0x00;
    spiDataWrite[2] = data;
    wiringPiSPIDataRW(ADS1298_chan, spiDataWrite, 3);
    digitalWrite(ADS1298_nCS,HIGH);
}

void DaqADS1298::sendCmd(uint8_t cmd)
{

    uint8_t spiDataCmd[1];
    spiDataCmd[0]=cmd;
    digitalWrite(ADS1298_nCS,LOW);
    //delayMicroseconds(1);
    wiringPiSPIDataRW(ADS1298_chan, spiDataCmd ,1);
    delay(10);
    digitalWrite(ADS1298_nCS,HIGH);
}

uint8_t DaqADS1298::readReg(uint8_t address)
{
    sendCmd(ADS1298_SDATAC);

    digitalWrite(ADS1298_nCS,LOW);
    uint8_t spiDataRead[3];
    spiDataRead[0] = ADS1298_RREG+address;
    spiDataRead[1] = 0x00;
    spiDataRead[2] = 0x00;
    //delayMicroseconds(1);
    wiringPiSPIDataRW(ADS1298_chan, spiDataRead, 3);
    delay(10);
    digitalWrite(ADS1298_nCS,HIGH);

    return spiDataRead[2];
}

void DaqADS1298::setup()
{
    //serv->printWriteLog("Starting ADS1298 setup...") ;
    qDebug() << "Starting ADS1298 setup...";
    int fclk = 3000000; //2.048MHz (max 20MHz)

    qDebug() <<  "Calling wiringPiSetupSys()";
    wiringPiSetupSys(); //init SPI pins

    int fd;
    fd=wiringPiSPISetup (ADS1298_chan, fclk); //init SPI pins

    uint8_t spiMode = SPI_MODE_1;
    qDebug() << "Setting SPI mode to 1";
    // Change to SPI mode 1 on write (default is 0)
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);

    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    pinMode(ADS1298_START, OUTPUT); //ADS1298_START
    pinMode(ADS1298_DRDY, INPUT); //ADS1298_DRDY
    pinMode(ADS1298_nRESET, OUTPUT); //_RESET
    pinMode(ADS1298_nCS, OUTPUT);
    pullUpDnControl (ADS1298_nCS, PUD_OFF);
    pullUpDnControl (ADS1298_MOSI, PUD_OFF);
    pullUpDnControl (ADS1298_MISO, PUD_OFF);
    pullUpDnControl (ADS1298_nRESET, PUD_UP);

    digitalWrite(ADS1298_START,LOW);
    //digitalWrite(ADS1298_nCS,LOW);
    // Power-up sequence
    qDebug() << "Power-up sequence";
    //digitalWrite(ADS1298_CLKSEL, HIGH);
    digitalWrite(ADS1298_nRESET,HIGH);
    delay(10);
    digitalWrite(ADS1298_nRESET,LOW);
    delay(1000);
    digitalWrite(ADS1298_nRESET,HIGH);

    // ADS1298_SDATAC (stop read data continuous mode) default mode
    qDebug() << "Sending ADS1298_SDATAC";
    sendCmd(ADS1298_SDATAC);

    //Read config file
    loadCfg();

    int len = 27;
    uint8_t spiDataWrite[len];

    /*Read config parameters and write to uC*/
    setting = config_lookup(&cfg, "registers");

    spiDataWrite[0] = ADS1298_WREG+1; // ADS1298_WREG +1 (address of CONFIG1)
    spiDataWrite[1] = 25 - 1; // Num of regs to write - 1

    int cfgtmp;
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
    digitalWrite(ADS1298_nCS,LOW);
    wiringPiSPIDataRW(ADS1298_chan, spiDataWrite, len);
    digitalWrite(ADS1298_nCS,HIGH);
    delay(100);

    qDebug() << "Reading config registers";
    printRegs();
    delay(100);

    qDebug() << "ADS1298 setup done.";
}

void DaqADS1298::printRegs(){

    qDebug() << "Printing values of registers";
    uint8_t res;
    int len = 26;

    for(int i=0; i<len;++i){

        res = readReg((uint8_t)i);
        qDebug() << "Register: " + QString::number(i) + " = " + QString::number(res);
    }
}

int DaqADS1298::getDrdyPin(){
    return DRDY;
}
