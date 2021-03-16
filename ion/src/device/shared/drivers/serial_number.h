#ifndef ION_DEVICE_SHARED_SERIAL_NUMBER_H
#define ION_DEVICE_SHARED_SERIAL_NUMBER_H

namespace Ion {
namespace Device {
namespace SerialNumber {

constexpr static int Length = 16;
const char * get();
void copy(char * buffer);

}
}
}

#endif
