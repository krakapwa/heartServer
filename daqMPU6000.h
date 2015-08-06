#ifndef DAQMPU6000_H
#define DAQMPU6000_H

#include <daq.h>
#include <dataMPU6000.h>

const int MPU6000_Nbytes = 12;
const int MPU6000_Nchannels = 6;

// register commands
const uint8_t MPUREG_XG_OFFS_TC = 0x00;
const uint8_t MPUREG_YG_OFFS_TC = 0x01;
const uint8_t MPUREG_ZG_OFFS_TC = 0x02;
const uint8_t MPUREG_X_FINE_GAIN = 0x03;
const uint8_t MPUREG_Y_FINE_GAIN = 0x04;
const uint8_t MPUREG_Z_FINE_GAIN = 0x05;
const uint8_t MPUREG_XA_OFFS_H = 0x06;
const uint8_t MPUREG_XA_OFFS_L = 0x07;
const uint8_t MPUREG_YA_OFFS_H = 0x08;
const uint8_t MPUREG_YA_OFFS_L = 0x09;
const uint8_t MPUREG_ZA_OFFS_H = 0x0A;
const uint8_t MPUREG_ZA_OFFS_L = 0x0B;
const uint8_t MPUREG_PRODUCT_ID = 0x0C;
const uint8_t MPUREG_SELF_TEST_X = 0x0D;
const uint8_t MPUREG_SELF_TEST_Y = 0x0E;
const uint8_t MPUREG_SELF_TEST_Z = 0x0F;
const uint8_t MPUREG_SELF_TEST_A = 0x10;
const uint8_t MPUREG_XG_OFFS_USRH = 0x13;
const uint8_t MPUREG_XG_OFFS_USRL = 0x14;
const uint8_t MPUREG_YG_OFFS_USRH = 0x15;
const uint8_t MPUREG_YG_OFFS_USRL = 0x16;
const uint8_t MPUREG_ZG_OFFS_USRH = 0x17;
const uint8_t MPUREG_ZG_OFFS_USRL = 0x18;
const uint8_t MPUREG_SMPLRT_DIV = 0x19;
const uint8_t MPUREG_CONFIG = 0x1A;
const uint8_t MPUREG_GYRO_CONFIG = 0x1B;
const uint8_t MPUREG_ACCEL_CONFIG = 0x1C;
const uint8_t MPUREG_INT_PIN_CFG = 0x37;
const uint8_t MPUREG_INT_ENABLE = 0x38;
const uint8_t MPUREG_ACCEL_XOUT_H = 0x3B;
const uint8_t MPUREG_ACCEL_XOUT_L = 0x3C;
const uint8_t MPUREG_ACCEL_YOUT_H = 0x3D;
const uint8_t MPUREG_ACCEL_YOUT_L = 0x3E;
const uint8_t MPUREG_ACCEL_ZOUT_H = 0x3F;
const uint8_t MPUREG_ACCEL_ZOUT_L = 0x40;
const uint8_t MPUREG_TEMP_OUT_H = 0x41;
const uint8_t MPUREG_TEMP_OUT_L = 0x42;
const uint8_t MPUREG_GYRO_XOUT_H = 0x43;
const uint8_t MPUREG_GYRO_XOUT_L = 0x44;
const uint8_t MPUREG_GYRO_YOUT_H = 0x45;
const uint8_t MPUREG_GYRO_YOUT_L = 0x46;
const uint8_t MPUREG_GYRO_ZOUT_H = 0x47;
const uint8_t MPUREG_GYRO_ZOUT_L = 0x48;
const uint8_t MPUREG_USER_CTRL = 0x6A;
const uint8_t MPUREG_PWR_MGMT_1 = 0x6B;
const uint8_t MPUREG_PWR_MGMT_2 = 0x6C;
const uint8_t MPUREG_BANK_SEL = 0x6D;
const uint8_t MPUREG_MEM_START_ADDR = 0x6E;
const uint8_t MPUREG_MEM_R_W = 0x6F;
const uint8_t MPUREG_DMP_CFG_1 = 0x70;
const uint8_t MPUREG_DMP_CFG_2 = 0x71;
const uint8_t MPUREG_FIFO_COUNTH = 0x72;
const uint8_t MPUREG_FIFO_COUNTL = 0x73;
const uint8_t MPUREG_FIFO_R_W = 0x74;
const uint8_t MPUREG_WHOAMI = 0x75;

// Configuration bits MPU6000
const uint8_t BIT_SLEEP = 0x40;
const uint8_t MPU_6000_PWR_MGMT_1_NO_SLEEP = 0x00;
const uint8_t MPU_6000_PWR_MGMT_1_SLEEP_bp = 0x06;
const uint8_t MPU_6000_PWR_MGMT_1_NO_RST = 0x00;
const uint8_t MPU_6000_PWR_MGMT_1_RST_bp =	0x07;
const uint8_t MPU_6000_PWR_MGMT_1_TEMP_DISABLE = 0x01;
const uint8_t MPU_6000_PWR_MGMT_1_TEMP_bp =	0x03;
const uint8_t BIT_H_RESET = 0x80;
const uint8_t BITS_CLKSEL = 0x07;
const uint8_t MPU_CLK_SEL_PLLGYROX = 0x01;
const uint8_t MPU_CLK_SEL_PLLGYROZ = 0x03;
const uint8_t MPU_EXT_SYNC_GYROX = 0x02;
const uint8_t BITS_FS_250DPS  =  0x00;
const uint8_t BITS_FS_500DPS  =  0x08;
const uint8_t BITS_FS_1000DPS  =  0x10;
const uint8_t BITS_FS_2000DPS  =  0x18;
const uint8_t BITS_FS_2G = 0x00;
const uint8_t BITS_FS_4G = 0x08;
const uint8_t BITS_FS_8G  =  0x10;
const uint8_t BITS_FS_16G  = 0x18;
const uint8_t BITS_FS_MASK  = 0x18;
const uint8_t BITS_DLPF_CFG_256HZ_NOLPF2  = 0x00;
const uint8_t BITS_DLPF_CFG_188HZ  =  0x01;
const uint8_t BITS_DLPF_CFG_98HZ =  0x02;
const uint8_t BITS_DLPF_CFG_42HZ =  0x03;
const uint8_t BITS_DLPF_CFG_20HZ = 0x04;
const uint8_t BITS_DLPF_CFG_10HZ = 0x05;
const uint8_t BITS_DLPF_CFG_5HZ = 0x06;
const uint8_t BITS_DLPF_CFG_2100HZ_NOLPF = 0x07;
const uint8_t BITS_DLPF_CFG_MASK  = 0x07;
const uint8_t BIT_INT_ANYRD_2CLEAR = 0x10;
const uint8_t BIT_RAW_RDY_EN = 0x01;
const uint8_t BIT_I2C_IF_DIS = 0x10;

#define READ_FLAG   0x80

// =======================================================================
// GPIO PINs
// =======================================================================
//const int DRDY = 3;
//const int nCS = 1;
//const int MOSI = 12;
//const int MISO = 13;
//const int chan = 1;



QT_USE_NAMESPACE

class DaqMPU6000: public Daq
{
    Q_OBJECT
public:
    DaqMPU6000();
    void setup();
    void setFclk(int);
    void setChan(int);
    void setMosiPin(int);
    void setMisoPin(int);
    void setNCsPin(int);
    void setSclkPin(int);
    void setFsyncPin(int);
    void setDrdyPin(int);
    std::vector<int32_t> *getData();
    int getNbytes();
    int getNchans();
    bool isPlugged;

signals:
    void sendMessageServer(QString);

private:
    std::string cfgFileName;
    void writeReg(uint8_t address, uint8_t data);
    void sendCmd(uint8_t cmd);
    uint8_t readReg(uint8_t address);
    void printRegs();
    int fclk;
    int DRDY;
    int nCS;
    int SCLK;
    int MOSI;
    int MISO;
    int FSYNC;
    uint8_t sample_rate_div;
    uint8_t low_pass_filter;

    DataMPU6000* y;
};


#endif // DAQMPU6000_H
