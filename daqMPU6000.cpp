#include <daqMPU6000.h>

void DaqMPU6000::setFclk(int arg_fclk){ fclk = arg_fclk;}
void DaqMPU6000::setChan(int arg_chan){ chan = arg_chan;}
void DaqMPU6000::setDrdyPin(int drdy){ DRDY = drdy;}
void DaqMPU6000::setSclkPin(int sclk){ SCLK = sclk;}
void DaqMPU6000::setMosiPin(int mosi){ MOSI = mosi;}
void DaqMPU6000::setMisoPin(int miso){ MISO = miso;}
void DaqMPU6000::setNCsPin(int ncs){ nCS = ncs;}
void DaqMPU6000::setFsyncPin(int fsync){FSYNC = fsync;}

DaqMPU6000::DaqMPU6000(){
    qDebug() << "MPU6000 contructor";
    y = new DataMPU6000();
}

void DaqMPU6000::getData(void){

    appendToFile(y);
}

void DaqMPU6000::appendToFile(DataMPU6000* y){

    myFile.write((char*)y->spiData, y->numSerialBytes*sizeof(uint8_t));
}

void DaqMPU6000::stopContinuous(){

    myFile.close();
}

void DaqMPU6000::startContinuous(QString fname){
    QString fnamePath;
    fname = "MPU6000" + fname;
    fnamePath = rootPath + fname;

    myFile.open(fnamePath.toStdString(), std::ios::in | std::ios::out | std::ios::binary);
    //myFile.setFileName(fnamePath);
    //myFile.open(QIODevice::WriteOnly | QIODevice::Append);
    //outStream.setDevice(&myFile);

}

void DaqMPU6000::writeReg(uint8_t address, uint8_t data)
{
    uint8_t spiDataWrite[2];
    spiDataWrite[0] = address;
    spiDataWrite[1] = data;
    digitalWrite(nCS,LOW);
    wiringPiSPIDataRW(chan, spiDataWrite, 2);
    digitalWrite(nCS,HIGH);
}

uint8_t DaqMPU6000::readReg(uint8_t address)
{

    uint8_t spiDataRead[2];
    spiDataRead[0] = address | READ_FLAG;
    spiDataRead[1] = 0x00;
    digitalWrite(nCS,LOW);
    wiringPiSPIDataRW(chan, spiDataRead, 2);
    digitalWrite(nCS,HIGH);

    return spiDataRead[1];
}

void DaqMPU6000::sendCmd(uint8_t cmd)
{

    uint8_t spiDataCmd[1];
    spiDataCmd[0]=cmd;
    digitalWrite(nCS,LOW);
    wiringPiSPIDataRW(chan, spiDataCmd ,1);
    delay(10);
    digitalWrite(nCS,HIGH);
}

void DaqMPU6000::setup ()
{
    //test
    delay(1000);

    uint8_t res;
    //serv->printWriteLog("Starting MPU6000 setup...") ;
    qDebug() << "Starting MPU6000 setup...";
    qDebug() << "chan: " + QString::number(chan);
    qDebug() << "fclk: " + QString::number(fclk);

    int fd;
    fd=wiringPiSPISetup(chan, fclk); //init SPI pins
    qDebug() << "wiringPiSPISetup on chan " + QString::number(chan) +  ": " + QString::number(fd);

    uint8_t spiMode = SPI_MODE_3; // Data captured on falling edge
    qDebug() << "Setting SPI mode to 3";
    // Change to SPI mode 0 on write (default is 0)
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);

    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    pinMode(nCS, OUTPUT);
    //pinMode(SCLK, OUTPUT);
    pullUpDnControl (nCS, PUD_OFF);
    pullUpDnControl (SCLK, PUD_OFF);
    pullUpDnControl (MOSI, PUD_DOWN);
    pullUpDnControl (MISO, PUD_DOWN);

    // Power-up sequence
    qDebug() << "Power-up sequence";

    //FIRST OF ALL DISABLE I2C
    qDebug() << "Disable I2C";
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);

    //RESET CHIP
    qDebug() << "Reset Chip";
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);
    writeReg(MPUREG_PWR_MGMT_1,BIT_H_RESET);
    delay(150);

    //WAKE UP AND SET GYROZ CLOCK
    qDebug() << "Wake up and set gyroz clock";
    writeReg(MPUREG_PWR_MGMT_1,MPU_CLK_SEL_PLLGYROZ);

    //DISABLE I2C
    qDebug() << "Disable I2C";
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);

    qDebug() << "WHOAMI?";
    delay(1000);
    //WHO AM I?
    res = readReg(MPUREG_WHOAMI);
    if(res<100){
        qDebug() << "Couldn't receive whoami: " + QString::number(res);
        //return;
    }
    else{
        qDebug() << "whoami result: " + QString::number(res);
        //qDebug() << "Received whoami";
    }
    delay(1000);

    //SET SAMPLE RATE TO 1kHz
    qDebug() << "Set sample rate";
    writeReg(MPUREG_SMPLRT_DIV,0x07);

    // DISABLE LPF
    qDebug() << "Disable LPF";
    writeReg(MPUREG_CONFIG,0x07);

    //DISABLE INTERRUPTS
    qDebug() << "Disable Interrupts";
    writeReg(MPUREG_INT_ENABLE,0x00);

    qDebug() << "MPU6000 setup done.";
}

void DaqMPU6000::printRegs(){

    qDebug() << "Printing values of registers";
    uint8_t res;
    int len = 26;

    for(int i=0; i<len;++i){

        res = readReg((uint8_t)i);
        qDebug() << "Register: " + QString::number(i) + " = " + QString::number(res);
    }
}
