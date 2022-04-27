#ifndef BOOTLOADER_RECOVERY_H_
#define BOOTLOADER_RECOVERY_H_

#include <stdint.h>
#include <stddef.h>

namespace Bootloader {
class Recovery {
 public:
  class CrashedSlot {
    public:
      CrashedSlot(const size_t size, const void * address) : m_storageAddress(address), m_storageSize(size) {}

      const size_t getStorageSize() const { return m_storageSize; }
      const void * getStorageAddress() const { return m_storageAddress; }

    private:
      const void * m_storageAddress;
      const size_t m_storageSize;
  };

  static CrashedSlot getSlotConcerned();

  static void crash_handler(const char * error);
  static void recoverData();
  static bool hasCrashed();

};
};

#endif //BOOTLOADER_RECOVERY_H_
