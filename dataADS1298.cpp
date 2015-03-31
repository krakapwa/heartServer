#include <dataADS1298.h>
#include <QDebug>

DataADS1298::DataADS1298() {
    numSerialBytes = nSerialBytesADS1298; //3-bytes header plus 3-bytes per channel
}
