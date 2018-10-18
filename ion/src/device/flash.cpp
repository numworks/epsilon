#include "flash.h"
#include <assert.h>

namespace Ion {
namespace Flash {
namespace Device {

static inline void wait() {
  // Wait for pending Flash operations to complete
  while (FLASH.SR()->getBSY()) {
  }
}

static void open() {
  // Unlock the Flash configuration register if needed
  if (FLASH.CR()->getLOCK()) {
    FLASH.KEYR()->set(0x45670123);
    FLASH.KEYR()->set(0xCDEF89AB);
  }
  assert(FLASH.CR()->getLOCK() == false);

  // Set the programming parallelism
  FLASH.CR()->setPSIZE(MemoryAccessWidth);
}

static void close() {
  // Lock the Flash configuration register
  assert(!FLASH.CR()->getMER());
  assert(!FLASH.CR()->getSER());
  assert(!FLASH.CR()->getPG());
  FLASH.CR()->setLOCK(true);

  // Purge Data and instruction cache
  if (FLASH.ACR()->getDCEN()) {
    FLASH.ACR()->setDCEN(false);
    FLASH.ACR()->setDCRST(true);
    FLASH.ACR()->setDCRST(false);
    FLASH.ACR()->setDCEN(true);
  }
  if (FLASH.ACR()->getICEN()) {
    FLASH.ACR()->setICEN(false);
    FLASH.ACR()->setICRST(true);
    FLASH.ACR()->setICRST(false);
    FLASH.ACR()->setICEN(true);
  }
}

static void typed_memcpy(uint8_t * source, uint8_t * destination, size_t length) {
  MemoryAccessType * src = reinterpret_cast<MemoryAccessType *>(source);
  MemoryAccessType * dst = reinterpret_cast<MemoryAccessType *>(destination);
  /* "length" gives the number of bytes of the memcpy, but operations on Flash
   * are made in MemoryAccessType. Make sure to write all the data if length is
   * not a multiple of MemoryAccessType.
   * For instance, if length is 5 bytes and memory operations are in uint32_t,
   * we need to make 2 write operations. */
  bool lengthIsMultipleOfMemoryAccessType = ((length*sizeof(uint8_t)) % sizeof(MemoryAccessType)) == 0;
  size_t numberOfWrites = (length*sizeof(uint8_t))/sizeof(MemoryAccessType) + (lengthIsMultipleOfMemoryAccessType ? 0 : 1);
  for (size_t i = 0; i < numberOfWrites; i++) {
    *dst++ = *src++;
  }
}

int SectorAtAddress(uint32_t address) {
  uint32_t sectorAddresses[NumberOfSectors] = {
    0x08000000, 0x08004000, 0x08008000, 0x0800C000,
    0x08010000, 0x08020000, 0x08040000, 0x08060000,
    0x08080000, 0x080A0000, 0x080C0000, 0x080E0000
  };
  for (int i=0; i<NumberOfSectors; i++) {
    if (sectorAddresses[i] == address) {
      return i;
    }
  }
  return -1;
}


void MassErase() {
  open();
  FLASH.CR()->setMER(true);
  FLASH.CR()->setSTRT(true);
  wait();
  FLASH.CR()->setMER(false);
  close();
}

void EraseSector(int i) {
  assert(i >= 0 && i < NumberOfSectors);
  open();
  FLASH.CR()->setSNB(i);
  FLASH.CR()->setSER(true);
  FLASH.CR()->setSTRT(true);
  wait();
  FLASH.CR()->setSNB(0);
  FLASH.CR()->setSER(false);
  close();
}

void WriteMemory(uint8_t * source, uint8_t * destination, size_t length) {
  open();
  FLASH.CR()->setPG(true);
  typed_memcpy(source, destination, length);
  wait();
  FLASH.CR()->setPG(false);
  close();
}

}
}
}
