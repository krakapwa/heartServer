#include <dataMPU6000.h>


DataMPU6000::DataMPU6000() {
    numSerialBytes = 2*8; //2 bytes per axis (acc + gyro)
    spiData[numSerialBytes] = {0};

}

