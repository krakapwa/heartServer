#include <dataADS1298.h>

DataADS1298::DataADS1298() {
    numSerialBytes = 3*8 + 3; //3-bytes header plus 3-bytes per channel
    spiData[numSerialBytes]={0};
}
