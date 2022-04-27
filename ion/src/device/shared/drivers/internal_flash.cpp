#include "internal_flash.h"
#include <drivers/cache.h>
#include <drivers/config/internal_flash.h>
#include <assert.h>
#include <algorithm>

namespace Ion {
namespace Device {
namespace InternalFlash {

using namespace Regs;

static inline void wait() {
  /* Issue a DSB instruction to guarantee the completion of a previous access
   * to FLASH_CR register or data write operation. (RM0431) */
  Cache::dsb();
  // Wait for pending Flash operations to complete
  while (FLASH.SR()->getBSY()) {
  }
}

static void open() {
  // Unlock the Flash configuration register if needed
  if (FLASH.CR()->getLOCK()) {
    // https://www.numworks.com/resources/engineering/hardware/electrical/parts/stm32f730-arm-mcu-reference-manual-1b6e1356.pdf#page=82
    FLASH.KEYR()->set(0x45670123);
    FLASH.KEYR()->set(0xCDEF89AB);
  }
  assert(FLASH.CR()->getLOCK() == false);

  // Set the programming parallelism
  FLASH.CR()->setPSIZE(MemoryAccessWidth);
}

static void open_protection() {
  if (FLASH.OPTCR()->getLOCK()) {
    // https://www.numworks.com/resources/engineering/hardware/electrical/parts/stm32f730-arm-mcu-reference-manual-1b6e1356.pdf#page=82
    FLASH.OPTKEYR()->set(0x08192A3B);
    FLASH.OPTKEYR()->set(0x4C5D6E7F);
  }
}

static void close_protection() {
  if(!FLASH.OPTCR()->getLOCK()) {
    FLASH.OPTCR()->setLOCK(true);
  }
}

static void disable_protection_at(int i) {
  if (!FLASH.OPTCR()->getLOCK()) {
    switch (i)
    {
    case 0:
      FLASH.OPTCR()->setnWRP0(true);
      break;
    case 1:
      FLASH.OPTCR()->setnWRP1(true);
      break;
    case 2:
      FLASH.OPTCR()->setnWRP2(true);
      break;
    case 3:
      FLASH.OPTCR()->setnWRP3(true);
      break;
    case 4:
      FLASH.OPTCR()->setnWRP4(true);
      break;
    case 5:
      FLASH.OPTCR()->setnWRP5(true);
      break;
    case 6:
      FLASH.OPTCR()->setnWRP6(true);
      break;
    case 7:
      FLASH.OPTCR()->setnWRP7(true);
      break;
    default:
      break;
    }
  }
}

static void enable_protection_at(int i) {
  if (!FLASH.OPTCR()->getLOCK()) {
    switch (i)
    {
    case 0:
      FLASH.OPTCR()->setnWRP0(false);
      break;
    case 1:
      FLASH.OPTCR()->setnWRP1(false);
      break;
    case 2:
      FLASH.OPTCR()->setnWRP2(false);
      break;
    case 3:
      FLASH.OPTCR()->setnWRP3(false);
      break;
    case 4:
      FLASH.OPTCR()->setnWRP4(false);
      break;
    case 5:
      FLASH.OPTCR()->setnWRP5(false);
      break;
    case 6:
      FLASH.OPTCR()->setnWRP6(false);
      break;
    case 7:
      FLASH.OPTCR()->setnWRP7(false);
      break;
    default:
      break;
    }
  }
}

static void close() {
  // Clear error flags
  class FLASH::SR sr(0);
  // Error flags are cleared by writing 1
  sr.setERSERR(true);
  sr.setPGPERR(true);
  sr.setPGAERR(true);
  sr.setWRPERR(true);
  sr.setEOP(true);
  FLASH.SR()->set(sr);

  // Lock the Flash configuration register
  assert(!FLASH.CR()->getMER());
  assert(!FLASH.CR()->getSER());
  assert(!FLASH.CR()->getPG());
  FLASH.CR()->setLOCK(true);

  // Purge Data and instruction cache
#if REGS_FLASH_CONFIG_ART
  if (FLASH.ACR()->getARTEN()) {
    FLASH.ACR()->setARTEN(false);
    FLASH.ACR()->setARTRST(true);
    FLASH.ACR()->setARTRST(false);
    FLASH.ACR()->setARTEN(true);
  }
#else
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
#endif
}

// Compile-time log2
static inline constexpr size_t clog2(size_t input) {
  return (input == 1) ? 0 : clog2(input/2)+1;
}

// Align a pointer to a given type's boundaries
// Returns a value that is lower or equal to input
template <typename T>
static inline T * align(void * input) {
  size_t k = clog2(sizeof(T));
  return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(input) & ~((1<<k) - 1));
}

template <typename T>
static inline T eat(void * ptr) {
  T * pointer = *reinterpret_cast<T **>(ptr);
  T result = *pointer;
  *reinterpret_cast<T **>(ptr) = pointer+1;
  return result;
}

static inline ptrdiff_t byte_offset(void * p1, void * p2) {
  return reinterpret_cast<uint8_t *>(p2) - reinterpret_cast<uint8_t *>(p1);
}

static void flash_memcpy(uint8_t * destination, uint8_t * source, size_t length) {
  /* RM0402 3.5.4
   * It is not allowed to program data to the Flash memory that would cross the
   * 128-bit row boundary. In such a case, the write operation is not performed
   * and a program alignment error flag (PGAERR) is set in the FLASH_SR
   * register.
   * The write access type (byte, half-word, word or double word) must
   * correspond to the type of parallelism chosen (x8, x16, x32 or x64). If not,
   * the write operation is not performed and a program parallelism error flag
   * (PGPERR) is set in the FLASH_SR register. */

  static_assert(
    sizeof(MemoryAccessType) == 1 ||
    sizeof(MemoryAccessType) == 2 ||
    sizeof(MemoryAccessType) == 4 ||
    sizeof(MemoryAccessType) == 8,
  "Invalid MemoryAccessType");

  /* So we may only perform memory writes with pointers of type MemoryAccessType
   * and we must make sure to never cross 128 bit boundaries. This second
   * requirement is satisfied iif the pointers are aligned on MemoryAccessType
   * boundaries.
   * Long story short: we want to perform writes to aligned(MemoryAccessType *).
   */

  /* Step 1 - Copy a header if needed
   * We start by copying a Header, whose size is MemoryAccessType, to bring us
   * back on aligned tracks.
   *
   *             _AlignedDst       _DESTINATION
   *            |                 |
   * --+--------+--------+--------+--------+--------+--------+--
   *   |        ||       |        |        |        ||       |
   *---+--------+--------+--------+--------+--------+--------+--
   *            |<------------ Header ------------->|
   *            |-- HeaderDelta ->|
   */

  MemoryAccessType * alignedDestination = align<MemoryAccessType>(destination);
  ptrdiff_t headerDelta = byte_offset(alignedDestination, destination);
  assert(headerDelta >= 0 && headerDelta < static_cast<ptrdiff_t>(sizeof(MemoryAccessType)));

  if (headerDelta > 0) {
    // At this point, alignedDestination < destination
    // We'll then retrieve the current value at alignedDestination, fill it with
    // bytes from source, and write it back at alignedDestination.

    // First, retrieve the current value at alignedDestination
    MemoryAccessType header = *alignedDestination;

    // Then copy headerLength bytes from source and put them in the header
    uint8_t * headerStart = reinterpret_cast<uint8_t *>(&header);
    // Here's where source data shall start being copied in the header
    uint8_t * headerDataStart = headerStart + headerDelta;
    // And here's where it should end
    uint8_t * headerDataEnd = std::min(
      headerStart + sizeof(MemoryAccessType), // Either at the end of the header
      headerDataStart + length // or whenever src runs out of data
    );
    for (uint8_t * h = headerDataStart; h<headerDataEnd; h++) {
      *h = eat<uint8_t>(&source);
    }

    // Then eventually write the header back into the aligned destination
    *alignedDestination++ = header;
    wait();
  }

  /* Step 2 - Copy the bulk of the data
   * At this point, we can use aligned MemoryAccessType pointers. */

  MemoryAccessType * lastAlignedDestination = align<MemoryAccessType>(destination + length);
  while (alignedDestination < lastAlignedDestination) {
    *alignedDestination++ = eat<MemoryAccessType>(&source);
    wait();
  }

  /* Step 3 - Copy a footer if needed
   * Some unaligned data can be pending at the end. Let's take care of it like
   * we did for the header.
   *
   *             _alignedDst       _Destination+length
   *            |                 |
   * --+--------+--------+--------+--------+--------+--------+--
   *   |        ||       |        |        |        ||       |
   *---+--------+--------+--------+--------+--------+--------+--
   *            |<------------ Footer ------------->|
   *            |- footerLength ->|
   */

  ptrdiff_t footerLength = byte_offset(alignedDestination, destination + length);
  assert(footerLength < static_cast<ptrdiff_t>(sizeof(MemoryAccessType)));
  if (footerLength > 0) {
    assert(alignedDestination == lastAlignedDestination);

    // First, retrieve the current value at alignedDestination
    MemoryAccessType footer = *alignedDestination;

    /* Then copy footerLength bytes from source and put them at the beginning of
     * the footer */
    uint8_t * footerPointer = reinterpret_cast<uint8_t *>(&footer);
    for (ptrdiff_t i=0; i<footerLength; i++) {
      footerPointer[i] = eat<uint8_t>(&source);
    }

    // Then eventually write the footer back into the aligned destination
    *alignedDestination = footer;
    wait();
  }
}

int SectorAtAddress(uint32_t address) {
  for (int i = 0; i < Config::NumberOfSectors; i++) {
    if (address >= Config::SectorAddresses[i] && address < Config::SectorAddresses[i+1]) {
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
  assert(i >= 0 && i < Config::NumberOfSectors);
  open();
  FLASH.CR()->setSNB(i);
  FLASH.CR()->setSER(true);
  FLASH.CR()->setSTRT(true);
  wait();
  FLASH.CR()->setSNB(0);
  FLASH.CR()->setSER(false);
  close();
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  open();
  FLASH.CR()->setPG(true);
  flash_memcpy(destination, source, length);
  FLASH.CR()->setPG(false);
  close();
}

void EnableProtection() {
  close_protection();
}

void DisableProtection() {
  open_protection();
}

void SetSectorProtection(int i, bool protect) {
  if (protect) {
    enable_protection_at(i);
  } else {
    disable_protection_at(i);
  }
}

void EnableSessionLock() {
  if (FLASH.OPTCR()->getLOCK()) {
    // writing bullshit to the lock register to lock it until next core reset
    // https://www.numworks.com/resources/engineering/hardware/electrical/parts/stm32f730-arm-mcu-reference-manual-1b6e1356.pdf#page=82
    // > "In the event of an unsuccessful unlock operation, this bit remains set until the next reset."
    FLASH.OPTKEYR()->set(0x00000000);
    FLASH.OPTKEYR()->set(0xFFFFFFFF);

    // Now, a bus fault error is triggered
  }
}

void EnableFlashInterrupt() {
  open();
  FLASH.CR()->setERRIE(true);
  wait();
  FLASH.CR()->setEOPIE(true);
  wait();
  FLASH.CR()->setRDERRIE(true);
  wait();
  close();
}

void ClearErrors() {
  class FLASH::SR sr(0);
  // Error flags are cleared by writing 1
  sr.setERSERR(true);
  sr.setPGPERR(true);
  sr.setPGAERR(true);
  sr.setWRPERR(true);
  sr.setEOP(true);
  FLASH.SR()->set(sr);
}

}
}
}
