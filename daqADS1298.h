#ifndef DAQADS1298_H
#define DAQADS1298_H
#include <data.h>
#include <daq.h>

const int numSerialBytes = 3*8 + 3; //3-bytes header plus 3-bytes per channel

// register commands
const uint8_t WREG = 0x40;
const uint8_t RREG  = 0x20;
const uint8_t RDATAC = 0x10;
const uint8_t SDATAC = 0x11;
const uint8_t RDATA = 0x12;

// =======================================================================
// CONFIG REGISTER
// =======================================================================

const uint8_t ADS1298_ID      = 0x00;
const uint8_t ADS1298_CONFIG1 = 0x01;
const uint8_t ADS1298_CONFIG2 = 0x02;
const uint8_t ADS1298_CONFIG3 = 0x03;
const uint8_t ADS1298_LOFF    = 0x04;
const uint8_t ADS1298_CH1SET = 0x04 + 1;
const uint8_t ADS1298_CH2SET = 0x04 + 2;
const uint8_t ADS1298_CH3SET = 0x04 + 3;
const uint8_t ADS1298_CH4SET = 0x04 + 4;
const uint8_t ADS1298_CH5SET = 0x04 + 5;
const uint8_t ADS1298_CH6SET = 0x04 + 6;
const uint8_t ADS1298_CH7SET = 0x04 + 7;
const uint8_t ADS1298_CH8SET = 0x04 + 8;
const uint8_t ADS1298_RLD_SENSP = 0x0d;
const uint8_t ADS1298_RLD_SENSN = 0x0e;
const uint8_t ADS1298_LOFF_SENSP = 0x0f;
const uint8_t ADS1298_LOFF_SENSN = 0x10;
const uint8_t ADS1298_LOFF_FLIP = 0x11;
const uint8_t ADS1298_LOFF_STATP = 0x12;
const uint8_t ADS1298_LOFF_STATN = 0x13;
const uint8_t ADS1298_GPIO = 0x14;
const uint8_t ADS1298_PACE = 0x15;
const uint8_t ADS1298_RESP = 0x16;
const uint8_t ADS1298_CONFIG4 = 0x17;
const uint8_t ADS1298_WCT1 = 0x18;
const uint8_t ADS1298_WCT2 = 0x19;

// =======================================================================
// GPIO PINs
// =======================================================================
const int START = 2;
const int DRDY = 3;
const int nRESET = 26;
const int CLKSEL = 2;
const int nCS = 1;
const int MOSI = 12;
const int MISO = 13;
const int chan = 0;


class Server;

QT_USE_NAMESPACE

class DaqADS1298: public Daq
{
    Q_OBJECT
public:
    void setup();


signals:
    void sendBuffer(Data);
    void sendMessageServer(QString);

private slots:
    void startContinuous(QString);
    void stopContinuous();

private:
    std::string cfgFileName = "configADS1298.txt";
    void writeReg(uint8_t address, uint8_t data);
    void sendCmd(uint8_t cmd);
    uint8_t readReg(uint8_t address);
    static void appendToFile(Data y);
    static void writeToBuffer(Data &y);
    static void getData();
    void printRegs();


};

#endif // DAQADS1298_H
