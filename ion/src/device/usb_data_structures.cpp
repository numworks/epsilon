#include "usb_data_structures.h"

namespace Ion {
namespace USB {
namespace Device {

DataDirection SetupDataHelper::bmRequestTypeDirection(uint8_t bmRequestType) {
  if (bmRequestType & 0x80) {
    return DataDirection::In;
  }
  return DataDirection::Out;
}

RequestTypeType SetupDataHelper::bmRequestTypeType(uint8_t bmRequestType) {
  return (RequestTypeType) ((bmRequestType & 0b01100000) >> 5);
}

int SetupDataHelper::descriptorIndexFromWValue(uint16_t wValue) {
  return wValue & 0xFF;
}

int SetupDataHelper::descriptorTypeFromWValue(uint16_t wValue) {
  return wValue >> 8;
}

}
}
}
