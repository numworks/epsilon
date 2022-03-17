#ifndef BOOTLOADER_KERNEL_HEADER_H
#define BOOTLOADER_KERNEL_HEADER_H

#include <stdint.h>

namespace Bootloader {

class KernelHeader {
public:
  const char * version() const;
  const char * patchLevel() const;
  const bool isValid() const;

  const uint32_t* stackPointer() const;
  const void(*startPointer() const)();

private:
  KernelHeader();
  constexpr static uint32_t Magic = 0xDEC00DF0;
  const uint32_t m_unknown;
  const uint32_t m_signature;
  const uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  const uint32_t m_footer;
  const uint32_t* m_stackPointer;
  const void(*m_startPointer)();
};

}

#endif
