﻿#include <daqMPU6000.h>

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


int DaqMPU6000::getNbytes(){
    return MPU6000_Nbytes;
}

static uint8_t buff[MPU6000_Nbytes];
uint8_t* DaqMPU6000::getData(void){

    //qDebug() << "getData MPU6000";

    //MPU6000
    uint8_t tmpSpiDataH[1] = {0};
    uint8_t tmpSpiDataL[1] = {0};

    //x axis accel
    tmpSpiDataH[0] =  readReg(MPUREG_ACCEL_XOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_ACCEL_XOUT_L );
    buff[0] = tmpSpiDataH[0];
    buff[1] = tmpSpiDataL[0];

    //y axis accel
    tmpSpiDataH[0] =  readReg(MPUREG_ACCEL_YOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_ACCEL_YOUT_L );
    buff[2] = tmpSpiDataH[0];
    buff[3] = tmpSpiDataL[0];

    //z axis accel
    tmpSpiDataH[0] =  readReg(MPUREG_ACCEL_ZOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_ACCEL_ZOUT_L );
    buff[4] = tmpSpiDataH[0];
    buff[5] = tmpSpiDataL[0];

    //x axis rot
    tmpSpiDataH[0] =  readReg(MPUREG_GYRO_XOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_GYRO_XOUT_L );
    buff[6] = tmpSpiDataH[0];
    buff[7] = tmpSpiDataL[0];

    //y axis rot
    tmpSpiDataH[0] =  readReg(MPUREG_GYRO_YOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_GYRO_YOUT_L );
    buff[8] = tmpSpiDataH[0];
    buff[9] = tmpSpiDataL[0];

    //z axis rot
    tmpSpiDataH[0] =  readReg(MPUREG_GYRO_ZOUT_H );
    tmpSpiDataL[0] =  readReg(MPUREG_GYRO_ZOUT_L );
    buff[10] = tmpSpiDataH[0];
    buff[11] = tmpSpiDataL[0];

    return (unsigned char*)&buff;
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

    /*
    uint8_t spiMode = SPI_MODE_3; // Data captured on falling edge
    qDebug() << "Setting SPI mode to 3";
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);

    */
    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    //pinMode(nCS, OUTPUT);
    //pinMode(SCLK, OUTPUT);
    //pullUpDnControl (nCS, PUD_OFF);
    //pullUpDnControl (SCLK, PUD_OFF);
    //pullUpDnControl (MOSI, PUD_DOWN);
    //pullUpDnControl (MISO, PUD_DOWN);

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
    writeReg(MPUREG_PWR_MGMT_1,0x00);

    qDebug() << "WHOAMI?";
    delay(1000);
    for(int i=0;i<1;++i){
      delay(1);
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
    }
    //delay(1000);

    //SET SAMPLE RATE TO 1kHz
    qDebug() << "Set sample rate";
    writeReg(MPUREG_SMPLRT_DIV,0x07);

    // DISABLE LPF
    qDebug() << "Disable LPF";
    //writeReg(MPUREG_CONFIG,0x07);
    writeReg(MPUREG_CONFIG,0x00);

    // Gyro scale 1000º/s
    writeReg(MPUREG_GYRO_CONFIG, BITS_FS_1000DPS);

    // Accel scele 2g (g=8192)
    writeReg(MPUREG_ACCEL_CONFIG, BITS_FS_2G);

    //DISABLE INTERRUPTS
    qDebug() << "Disable Interrupts";
    writeReg(MPUREG_INT_ENABLE,0x00);

    // Read LPF reg
    qDebug() << "LPF value:";
    uint8_t lpf;
    lpf = readReg(MPUREG_CONFIG);
    qDebug() << QString::number(lpf);

    //FIRST OF ALL DISABLE I2C
    qDebug() << "Disable I2C";
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);

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
