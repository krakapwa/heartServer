#ifndef DATA_H
#define DATA_H

#include <stdint.h>

class Data {

public:

   int numSerialBytes; //3-bytes header plus 3-bytes per channel
    uint8_t spiData[];
};
#endif // DATA_H
