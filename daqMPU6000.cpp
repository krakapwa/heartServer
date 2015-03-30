#include <daqMPU6000.h>

void DaqMPU6000::setFclk(int fclk){ fclk = fclk;}
void DaqMPU6000::setChan(int chan){ chan = chan;}
void DaqMPU6000::setDrdyPin(int drdy){ DRDY = drdy;}
void DaqMPU6000::setMosiPin(int mosi){ MOSI = mosi;}
void DaqMPU6000::setMisoPin(int miso){ MISO = miso;}
void DaqMPU6000::setNCsPin(int ncs){ nCS = ncs;}


DaqMPU6000::DaqMPU6000(){
    qDebug() << "MPU6000 contructor";
    y = new DataMPU6000();
    qDebug() << "hi";
}

void DaqMPU6000::getData(void){

    uint8_t tmpSpiDataH[1];
    uint8_t tmpSpiDataL[1] = {0};

    //x axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_XOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[0] = tmpSpiDataH[0];
    y->spiData[1] = tmpSpiDataL[0];

    //y axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_YOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[0] = tmpSpiDataH[0];
    y->spiData[1] = tmpSpiDataL[0];

    //z axis accel
    tmpSpiDataH[0] = MPUREG_ACCEL_ZOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[2] = tmpSpiDataH[0];
    y->spiData[3] = tmpSpiDataL[0];

    //x axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_XOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[4] = tmpSpiDataH[0];
    y->spiData[5] = tmpSpiDataL[0];

    //y axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_YOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[6] = tmpSpiDataH[0];
    y->spiData[7] = tmpSpiDataL[0];

    //z axis rot
    tmpSpiDataH[0] = MPUREG_GYRO_ZOUT_H | READ_FLAG;
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataH , 1);
    wiringPiSPIDataRW(chan, tmpSpiDataL , 1);
    y->spiData[8] = tmpSpiDataH[0];
    y->spiData[9] = tmpSpiDataL[0];

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
    delay(10);
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
    uint8_t res;
    //serv->printWriteLog("Starting MPU6000 setup...") ;
    qDebug() << "Starting MPU6000 setup...";
    fclk = 1000000; //2.048MHz (max 20MHz)

    qDebug() <<  "Calling wiringPiSetup()";
    wiringPiSetup(); //init SPI pins

    int fd;
    fd=wiringPiSPISetup (chan, fclk); //init SPI pins

    uint8_t spiMode = SPI_MODE_0; // Data captured on falling edge
    qDebug() << "Setting SPI mode to 0";
    // Change to SPI mode 0 on write (default is 0)
    ioctl (fd, SPI_IOC_WR_MODE, &spiMode);

    qDebug() << "Setting up pins";
    // Setup gpio pin modes for SPI
    pinMode(nCS, OUTPUT);
    pullUpDnControl (nCS, PUD_DOWN) ;
    pullUpDnControl (MOSI, PUD_OFF) ;
    pullUpDnControl (MISO, PUD_OFF) ;


    // Power-up sequence
    qDebug() << "Power-up sequence";

    //FIRST OF ALL DISABLE I2C
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);

    //RESET CHIP
    writeReg(MPUREG_PWR_MGMT_1,BIT_H_RESET);
    delay(150);

    //WAKE UP AND SET GYROZ CLOCK
    writeReg(MPUREG_PWR_MGMT_1,MPU_CLK_SEL_PLLGYROZ);

    //DISABLE I2C
    writeReg(MPUREG_USER_CTRL,BIT_I2C_IF_DIS);

    //WHO AM I?
    res = readReg(MPUREG_WHOAMI);
    if(res<100){
        qDebug() << "Couldn't receive whoami";
        return;
    }
    else{
       qDebug() << "Received whoami";
    }

    //SET SAMPLE RATE TO 1kHz
    writeReg(MPUREG_SMPLRT_DIV,0x07);

    // DISABLE LPF
    writeReg(MPUREG_CONFIG,0x07);

    //DISABLE INTERRUPTS
    writeReg(MPUREG_INT_ENABLE,0x00);

    //Read config file
    loadCfg();

    /*Read config parameters and write to uC*/
    setting = config_lookup(&cfg, "registers");

    config_destroy(&cfg);

    delay(100);

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

