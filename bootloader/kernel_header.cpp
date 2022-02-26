#include <bootloader/kernel_header.h>

namespace Bootloader {

const char * KernelHeader::version() const {
  return m_version;
}

const char * KernelHeader::patchLevel() const {
  return m_patchLevel;
}

const bool KernelHeader::isValid() const {
  return m_header == Magic && m_footer == Magic;
}

const uint32_t* KernelHeader::stackPointer() const {
  return m_stackPointer;
}

const void(*KernelHeader::startPointer() const)() {
  return m_startPointer;
}

}
