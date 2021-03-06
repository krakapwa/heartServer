#include "daqADS1298.h"

//static uint8_t tmp[nSerialBytes];

DaqADS1298::DaqADS1298(){
    DRDY = ADS1298_DRDY;
    //tmpSpiData[nSerialBytes] = {0};
}

void DaqADS1298::startContinuous(){

    digitalWrite(ADS1298_START,HIGH);
    sendCmd(ADS1298_RDATAC);
}

void DaqADS1298::stopContinuous(){

    digitalWrite(ADS1298_START,LOW);
    delay(1);
}

void DaqADS1298::writeReg(uint8_t address, uint8_t data)
{

    // ADS1298_SDATAC (stop read data continuous mode) default mode
    uint8_t spiDataCmd[2];
    spiDataCmd[0] = ADS1298_SDATAC;

    digitalWrite(ADS1298_nCS,LOW);

    delay(10);
    wiringPiSPIDataRW(ADS1298_chan, spiDataCmd ,1);

    uint8_t spiDataWrite[3];
    spiDataWrite[0] = ADS1298_WREG+address;
    spiDataWrite[1] = 0x00;
    spiDataWrite[2] = data;
    wiringPiSPIDataRW(ADS1298_chan, spiDataWrite, 3);
    delay(10);
    digitalWrite(ADS1298_nCS,HIGH);
}

void DaqADS1298::sendCmd(uint8_t cmd)
{

    uint8_t spiDataCmd[1];
    spiDataCmd[0]=cmd;
    digitalWrite(ADS1298_nCS,LOW);
    delay(10);
    wiringPiSPIDataRW(ADS1298_chan, spiDataCmd ,1);
    delay(10);
    digitalWrite(ADS1298_nCS,HIGH);
}

uint8_t DaqADS1298::readReg(uint8_t address)
{
    sendCmd(ADS1298_SDATAC);

    digitalWrite(ADS1298_nCS,LOW);
    delay(10);
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
    chan = ADS1298_chan;
    fclk = 1000000; //2.048MHz (max 20MHz)

    int fd;
    fd=wiringPiSPISetup(ADS1298_chan, fclk); //init SPI pins
    qDebug() << "wiringPiSPISetup on chan " + QString::number(ADS1298_chan) +  ": " + QString::number(fd);

    uint8_t spiMode = SPI_MODE_1;
    qDebug() << "Setting SPI mode to 1";
    // Change to SPI mode 1 on write (default is 0)
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);


    //wiringPiSetupSys();
    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    pinMode(ADS1298_START, OUTPUT); //ADS1298_START
    pinMode(ADS1298_DRDY, INPUT); //ADS1298_DRDY
    pinMode(ADS1298_nRESET, OUTPUT); //_RESET
    //pinMode(ADS1298_nCS, OUTPUT);
    //pinMode(ADS1298_MOSI, OUTPUT);
    //pinMode(ADS1298_MISO, INPUT);

    //pullUpDnControl (ADS1298_nCS, PUD_OFF);
    //pullUpDnControl (ADS1298_MOSI, PUD_OFF);
    //pullUpDnControl (ADS1298_MISO, PUD_OFF);
    pullUpDnControl (ADS1298_nRESET, PUD_OFF);
    pullUpDnControl (ADS1298_DRDY, PUD_UP);

    digitalWrite(ADS1298_START,LOW);
    //digitalWrite(ADS1298_nCS,LOW);
    // Power-up sequence
    qDebug() << "Power-up sequence";
    //digitalWrite(ADS1298_CLKSEL, HIGH);
    //digitalWrite(ADS1298_nRESET,HIGH);
    //delay(10);
    digitalWrite(ADS1298_nRESET,LOW);
    delay(3);
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


    //config_destroy(&cfg);

    //Save Fs
    setFsFromCfg();

    //Write registers
    qDebug() << "Writing config registers";
    digitalWrite(ADS1298_nCS,LOW);
    delay(100);
    wiringPiSPIDataRW(ADS1298_chan, spiDataWrite, len);
    digitalWrite(ADS1298_nCS,HIGH);
    delay(50);

    qDebug() << "Reading config registers";
    printRegs();

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

int DaqADS1298::getFclk(){
    return fclk;
}

int DaqADS1298::getMisoPin(){
    return ADS1298_MISO;
}

int DaqADS1298::getMosiPin(){
    return ADS1298_MOSI;
}

int DaqADS1298::getSclkPin(){
    return ADS1298_sclk;
}

int DaqADS1298::getNbytes(){
    return ADS1298_Nbytes;
}

void DaqADS1298::setFsFromCfg(){
    //Save Fs
    int cfgtmp;
    int reg[CHAR_BIT];
    config_setting_lookup_int(setting, "CONFIG1", &cfgtmp);
    QByteArray cfg1 = QByteArray::number(cfgtmp,10);
    char *data = cfg1.data();
    //qDebug() << "Save Fs";
    for (int i = 0; i < CHAR_BIT; ++i) {
      reg[i] = (*data >> i) & 1;
      //qDebug() << QString::number(reg[i]);
    }

    int nibble = reg[0] + 2*reg[1] + 4*reg[2];
    if(reg[7]){ //High resolution mode
        if(nibble==0) fs=32000;
        if(nibble==1) fs=16000;
        if(nibble==2) fs=8000;
        if(nibble==3) fs=4000;
        if(nibble==4) fs=2000;
        if(nibble==5) fs=1000;
        if(nibble==6) fs=500;
    }
    else{

        if(nibble==0) fs=16000;
        if(nibble==1) fs=8000;
        if(nibble==2) fs=4000;
        if(nibble==3) fs=2000;
        if(nibble==4) fs=1000;
        if(nibble==5) fs=500;
        if(nibble==6) fs=250;
    }

}

static std::vector<int32_t> bufferADS1298(ADS1298_Nchannels);
std::vector<int>* DaqADS1298::getData(){
    //qDebug() << "getData ADS1298";
    //int chan = daqs[0]
    uint8_t tmp[ADS1298_Nbytes] = {0};
    //    getWriteData(&(daqs[0]->myFile),8, 0, 27);
    digitalWrite(ADS1298_nCS,LOW);
    wiringPiSPIDataRW(ADS1298_chan, tmp ,ADS1298_Nbytes);
    digitalWrite(ADS1298_nCS,HIGH);

    // 3 bytes per channel
    for( int i=0; i < ADS1298_Nbytes/3; ++i ){
       bufferADS1298[i] =  uint32Toint32((tmp[3*i]<<16) + (tmp[3*i+1]<<8) + (tmp[3*i+2]));
    }

    return &bufferADS1298;
}

int32_t DaqADS1298::uint32Toint32(uint32_t in){
    if(in & 0x800000){
        in |= ~0xffffff;
    }

    return (int32_t)in;
}

int DaqADS1298::getNchans(){
   return ADS1298_Nchannels;
}
