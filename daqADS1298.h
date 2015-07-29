#ifndef DAQADS1298_H
#define DAQADS1298_H
#include <dataADS1298.h>
#include <daq.h>

//Global
const int ADS1298_Nbytes = 9; //3 header bytes + 2 channel
const int ADS1298_Nchannels = 3;

// register commands
const uint8_t ADS1298_WREG = 0x40;
const uint8_t ADS1298_RREG  = 0x20;
const uint8_t ADS1298_RDATAC = 0x10;
const uint8_t ADS1298_SDATAC = 0x11;
const uint8_t ADS1298_RDATA = 0x12;

// =======================================================================
// GPIO PINs (BCM convention with wiringPiSetup)
// =======================================================================
const int ADS1298_START = 27;
const int ADS1298_DRDY = 22;
const int ADS1298_nRESET = 12;
const int ADS1298_nCS = 8;
const int ADS1298_MOSI = 10;
const int ADS1298_MISO = 9;
const int ADS1298_chan = 0;
const int ADS1298_sclk = 11;

QT_USE_NAMESPACE

class DaqADS1298: public Daq
{
    Q_OBJECT
public:
    DaqADS1298();
    void setup();
    int getDrdyPin(void);
    int getMisoPin();
    int getMosiPin();
    int getSclkPin();
    int getFclk(void);
    std::vector<int> *getData();
    void setFsFromCfg();
    int getNbytes();
    int getNchans();

signals:
    void sendBuffer(DataADS1298);
    void sendDataToServer(DaqADS1298*, DataADS1298);
    void sendMessageServer(QString);

private slots:
    void startContinuous();
    void stopContinuous();

private:
    std::string cfgFileName;
    void writeReg(uint8_t address, uint8_t data);
    void sendCmd(uint8_t cmd);
    int32_t uint32Toint32(uint32_t in);
    uint8_t readReg(uint8_t address);
    void writeToBuffer(DataADS1298*);
    void printRegs();
    int DRDY;
    int fclk; //2.048MHz (max 20MHz)
    DataADS1298* y;
};

#endif // DAQADS1298_H
