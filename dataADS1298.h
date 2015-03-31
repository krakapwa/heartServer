#ifndef DATAADS1298_H
#define DATAADS1298_H
#include <stdint.h>

const int nSerialBytesADS1298 = 27;

class DataADS1298 {

public:

    DataADS1298();
    int numSerialBytes;
    uint8_t spiData[nSerialBytesADS1298];
};

#endif // DATA_H

