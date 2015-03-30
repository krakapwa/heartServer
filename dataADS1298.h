#ifndef DATAADS1298_H
#define DATAADS1298_H
#include <stdint.h>

const int nSerialBytes = 27;

class DataADS1298 {

public:

    DataADS1298();
    int numSerialBytes;
    uint8_t spiData[nSerialBytes];
};

#endif // DATA_H

