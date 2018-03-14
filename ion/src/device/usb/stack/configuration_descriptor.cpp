#include "configuration_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

uint16_t ConfigurationDescriptor::copy(void * target, size_t maxSize) const {
  uint16_t size = Descriptor::copy(target, maxSize);
  for (uint8_t i = 0; i < m_bNumInterfaces; i++) {
    size += (m_interfaces+i)->copy(((uint8_t *)target) + DescriptorSize + i*sizeof(InterfaceDescriptor), maxSize - DescriptorSize - i*sizeof(InterfaceDescriptor));
  }
  return size;
}

}
}
}
