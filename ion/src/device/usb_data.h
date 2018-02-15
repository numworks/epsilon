#ifndef ION_DEVICE_USB_DATA_H
#define ION_DEVICE_USB_DATA_H

extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace USB {
namespace Device {

#define USB_LANGID_ENGLISH_US 0x0409

class USBData {
public:
  constexpr static const char * k_strings[] = {
    "NumWorks",
    "Calculatrice USB",
    "111111111",
    "www.numworks.com/fr/"
  };

  constexpr static uint8_t k_numberOfStrings = 4;
};

}
}
}

#endif
