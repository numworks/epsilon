#include <bootloader/kernel_header.h>

extern "C" void jump_to_firmware(const uint32_t* stackPtr, const void(*startPtr)(void));

namespace Bootloader {


const KernelHeader* s_kernelHeaderA = reinterpret_cast<const struct KernelHeader*>(0x90000000);
const KernelHeader* s_kernelHeaderB = reinterpret_cast<const struct KernelHeader*>(0x90400000);

const char * KernelHeader::version() const {
  return m_version;
}

const char * KernelHeader::patchLevel() const {
  return m_patchLevel;
}

const bool KernelHeader::isValid() const {
  return m_header == Magic && m_footer == Magic;
}

[[ noreturn ]] void KernelHeader::boot() const {
  jump_to_firmware(m_stackPointer, m_startPointer);
  for(;;);
}


}
