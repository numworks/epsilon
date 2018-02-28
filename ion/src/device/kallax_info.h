#ifndef ION_DEVICE_KALLAX_INFO_H
#define ION_DEVICE_KALLAX_INFO_H

#include <ion/kallax.h>

namespace Ion {
namespace Device {

class KallaxInfo {
public:
  constexpr KallaxInfo(uint32_t address) :
    m_header(Magic),
    m_address(address),
    m_footer(Magic) { }
private:
  constexpr static uint32_t Magic = 0xDECB0DF0;
  uint32_t m_header;
  uint32_t m_address;
  uint32_t m_footer;
};

}
}

#endif

