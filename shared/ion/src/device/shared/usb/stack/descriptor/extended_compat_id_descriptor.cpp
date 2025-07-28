#include "extended_compat_id_descriptor.h"

#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

ExtendedCompatIDDescriptor::ExtendedCompatIDDescriptor(const char* compatibleID)
    : m_dwLength(sizeof(uint32_t) + 2 * sizeof(uint16_t) + sizeof(uint8_t) +
                 k_reserved1Size * sizeof(uint8_t) + 2 * sizeof(uint8_t) +
                 k_compatibleIDSize * sizeof(uint8_t) +
                 k_compatibleIDSize * sizeof(uint8_t) +
                 k_reserved2Size * sizeof(uint8_t)),
      m_bcdVersion(0x0100),  // Microsoft OS Descriptors version 1
      m_wIndex(Index),
      m_bCount(1),  // We assume one function only.
      m_reserved1{0, 0, 0, 0, 0, 0, 0},
      m_bFirstInterfaceNumber(0),
      m_bReserved(1),
      m_subCompatibleID{0, 0, 0, 0, 0, 0, 0, 0},
      m_reserved2{0, 0, 0, 0, 0, 0} {
  /* Compatible ID has size k_compatibleIDSize, and any unused bytes should be
   * filled with 0. */
  size_t compatibleIDSize = strlen(compatibleID);
  size_t compatibleIDCopySize = k_compatibleIDSize < compatibleIDSize
                                    ? k_compatibleIDSize
                                    : compatibleIDSize;
  for (size_t i = 0; i < compatibleIDCopySize; i++) {
    m_compatibleID[i] = compatibleID[i];
  }
  for (size_t i = compatibleIDCopySize; i < k_compatibleIDSize; i++) {
    m_compatibleID[i] = 0;
  }
}

void ExtendedCompatIDDescriptor::push(Channel* c) const {
  c->push(m_dwLength);
  c->push(m_bcdVersion);
  c->push(m_wIndex);
  c->push(m_bCount);
  for (uint8_t i = 0; i < k_reserved1Size; i++) {
    c->push(m_reserved1[i]);
  }
  c->push(m_bFirstInterfaceNumber);
  c->push(m_bReserved);
  for (uint8_t i = 0; i < k_compatibleIDSize; i++) {
    c->push(m_compatibleID[i]);
  }
  for (uint8_t i = 0; i < k_compatibleIDSize; i++) {
    c->push(m_subCompatibleID[i]);
  }
  for (uint8_t i = 0; i < k_reserved2Size; i++) {
    c->push(m_reserved2[i]);
  }
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
