#include <bootloader/slots/kernel_header.h>
#include <bootloader/utility.h>

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

const bool KernelHeader::isAboveVersion16 () const {
  int sum = Utility::versionSum(m_version, 2);
  char newVersion[] = "16";
  int min = Utility::versionSum(newVersion, 2);
  return sum >= min;
}


}
