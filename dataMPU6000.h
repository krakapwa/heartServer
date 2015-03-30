#ifndef DATAMPU6000_H
#define DATAMPU6000_H

#include <stdint.h>

class DataMPU6000 {

public:

    DataMPU6000();
    int numSerialBytes;
    uint8_t spiData[];
};


#endif // DATAMPU6000_H
