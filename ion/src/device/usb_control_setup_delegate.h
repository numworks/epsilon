#ifndef ION_DEVICE_USB_CONTROL_SETUP_DELEGATE_H
#define ION_DEVICE_USB_CONTROL_SETUP_DELEGATE_H

#include "usb_data_structures.h"

namespace Ion {
namespace USB {
namespace Device {

class ControlSetupDelegate {
public:
  virtual int buildDataToSend(SetupData setupData, uint8_t ** bufferToSend, uint16_t * bufferToSendLength, uint8_t * bufferForWriting) = 0;
};

}
}
}

#endif
