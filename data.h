#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <QVector>

class Data {

public:

   int numSerialBytes; //3-bytes header plus 3-bytes per channel
};
#endif // DATA_H
