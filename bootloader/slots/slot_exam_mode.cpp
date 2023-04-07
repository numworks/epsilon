#include <bootloader/slots/slot_exam_mode.h>
#include <assert.h>
#include <ion.h>
#include <ion/src/device/shared/drivers/flash.h>

namespace Bootloader {
namespace ExamMode {

/* The exam mode is written in flash so that it is resilient to resets.
 * We erase the dedicated flash sector (all bits written to 1) and, upon
 * deactivating or activating standard, nosym or Dutch exam mode we write one, two or tree
 * bits to 0. To determine in which exam mode we are, we count the number of
 * leading 0 bits. If it is equal to:
 * - 0[3]: the exam mode is off;
 * - 1[3]: the standard exam mode is activated;
 * - 2[3]: the NoSym exam mode is activated;
 * - 3[3]: the Dutch exam mode is activated;
 * - 4[3]: the NoSymNoText exam mode is activated. */

/* significantExamModeAddress returns the first uint32_t * in the exam mode
 * flash sector that does not point to 0. If this flash sector has only 0s or
 * if it has only one 1, it is erased (to 1) and significantExamModeAddress
 * returns the start of the sector. */

constexpr static size_t numberOfBitsInByte = 8;

// if i = 0b000011101, firstOneBitInByte(i) returns 5
size_t numberOfBitsAfterLeadingZeroes(int i) {
  int minShift = 0;
  int maxShift = numberOfBitsInByte;
  while (maxShift > minShift+1) {
    int shift = (minShift + maxShift)/2;
    int shifted = i >> shift;
    if (shifted == 0) {
      maxShift = shift;
    } else {
      minShift = shift;
    }
  }
  return maxShift;
}

uint8_t SlotsExamMode::FetchSlotExamMode(const char * version, const char * Slot) {
  // Get start and end from version and slot
  uint32_t start = 0;
  uint32_t end = 0;
  if (Slot == "A") {
    // If version under 16 get old addresses
    if (version[0] < '1' || (version[0] == '1' && version[1] < '6')) {
      start = getSlotAStartExamAddress(0);
      end = getSlotAEndExamAddress(0);
    }
    // Else get new addresses
    else {
      start = getSlotAStartExamAddress(1);
      end = getSlotAEndExamAddress(1);
    }
  }
  else if (Slot == "B") {
    // If version under 16 get old
    if (version[0] < '1' || (version[0] == '1' && version[1] < '6')) {
      start = getSlotBStartExamAddress(0);
      end = getSlotBEndExamAddress(0);
    }
    // Else get new
    else {
      start = getSlotBStartExamAddress(1);
      end = getSlotBEndExamAddress(1);
    }
  } else if (Slot == "Khi") {
    // We directly get the address of the Khi exam mode without checking the
    // version, because on Khi, version is KhiCAS version, not the OS version
    start = getSlotKhiStartExamAddress();
    end = getSlotKhiEndExamAddress();
  }

  if (strcmp("15.9.0", version) >= 0) {
    return examFetch15(start, end);
  } else if (strcmp("16.9.0", version) > 0) {
    return examFetch16(start, end);
  } else if (strcmp("19.0.0", version) > 0) {
    return examFetch1718(start, end);
  } else {
    return examFetch19(start, end);
  }
}



uint8_t SlotsExamMode::FetchSlotAExamMode(const char* version) {
  return FetchSlotExamMode(version, "A");
}

uint8_t SlotsExamMode::FetchSlotBExamMode(const char* version) {
  return FetchSlotExamMode(version, "B");
}

uint8_t SlotsExamMode::FetchSlotKhiExamMode(const char* version) {
  return FetchSlotExamMode(version, "Khi");
}

uint8_t SlotsExamMode::examFetch15(uint32_t start, uint32_t end) {
  uint32_t * persitence_start_32 = (uint32_t  *)start;
  uint32_t * persitence_end_32 = (uint32_t *)end;
  assert((persitence_end_32 - persitence_start_32) % 4 == 0);
  while (persitence_start_32 < persitence_end_32 && *persitence_start_32 == 0x0) {
    // Scan by groups of 32 bits to reach first non-zero bit
    persitence_start_32++;
  }
  uint8_t * persitence_start_8 = (uint8_t *)persitence_start_32;
  uint8_t * persitence_end_8 = (uint8_t *)persitence_end_32;
  while (persitence_start_8 < persitence_end_8 && *persitence_start_8 == 0x0) {
    // Scan by groups of 8 bits to reach first non-zero bit
    persitence_start_8++;
  }
  if (persitence_start_8 == persitence_end_8
  // we can't toggle from 0[3] to 2[3] when there is only one 1 bit in the whole sector
  || (persitence_start_8 + 1 == persitence_end_8 && *persitence_start_8 == 1)) {
    assert(Ion::Device::Flash::SectorAtAddress(start) >= 0);
    Ion::Device::Flash::EraseSector(start);
    uint32_t nbOfZerosBefore = (((uint8_t*)start - (uint8_t*)start) * numberOfBitsInByte) % 4;
    // Count the number of 0[3] at reading address
    size_t numberOfLeading0 = (numberOfBitsInByte - numberOfBitsAfterLeadingZeroes(*(uint8_t*)start)) % 4;
    return (nbOfZerosBefore + numberOfLeading0) % 4;
  }
  uint32_t nbOfZerosBefore = ((persitence_start_8 - (uint8_t*)start) * numberOfBitsInByte) % 4;
    // Count the number of 0[3] at reading address
    size_t numberOfLeading0 = (numberOfBitsInByte - numberOfBitsAfterLeadingZeroes(*persitence_start_8)) % 4;
    return (nbOfZerosBefore + numberOfLeading0) % 4;
}

uint8_t SlotsExamMode::examFetch16(uint32_t start, uint32_t end) {
  uint8_t* persitence_start_8 = (uint8_t*)start;
  uint8_t* persitence_end_8 = (uint8_t*)end;
  while (persitence_start_8 + 1 <= persitence_end_8 && (*persitence_start_8 != 0xFF)) {
    // Scan by groups of 8 bits to reach first non-zero bit
    persitence_start_8++;
  }

  return *(persitence_start_8 - 1);
}

uint8_t SlotsExamMode::examFetch1718(uint32_t start, uint32_t end) {
  uint8_t* persitence_start_8 = (uint8_t*)start;
  uint8_t* persitence_end_8 = (uint8_t*)end;
  while (persitence_start_8 + 1 <= persitence_end_8 && (*persitence_start_8 != 0xFF)) {
    // Scan by groups of 8 bits to reach first non-zero bit
    persitence_start_8++;
}

  return *(persitence_start_8 - 2);
}

uint8_t SlotsExamMode::examFetch19(uint32_t start, uint32_t end) {
  uint16_t* start16 = (uint16_t*)start;
  uint16_t* end16 = (uint16_t*)end;

  for (uint16_t* i = end16 - 2; i > start16; i--) {
    if (*i != 0xFFFF)  {
      uint8_t highByte = *i >> 8;
      uint8_t lowByte = *i & 0xFF;
      if (highByte > lowByte) {
        return highByte;
      }
      else {
        return lowByte;
      }
    }
  }
}

uint32_t SlotsExamMode::getSlotAStartExamAddress(int ExamVersion) {
  if (ExamVersion == 0) {
        return SlotAExamModeBufferStartOldVersions;
  }
  else {
    return SlotAExamModeBufferStartNewVersions;
  }
}

uint32_t SlotsExamMode::getSlotAEndExamAddress(int ExamVersion) {
  if (ExamVersion == 0) {
    return SlotAExamModeBufferEndOldVersions;
  }
  else {
        return SlotAExamModeBufferEndNewVersions;;
  }
}

uint32_t SlotsExamMode::getSlotBStartExamAddress(int ExamVersion) {
  if (ExamVersion == 0) {
    return SlotBExamModeBufferStartOldVersions;
  }
  else {
    return SlotBExamModeBufferStartNewVersions;
  }
}

uint32_t SlotsExamMode::getSlotBEndExamAddress(int ExamVersion) {
  if (ExamVersion == 0) {
    return SlotBExamModeBufferEndOldVersions;
  }
  else {
    return SlotBExamModeBufferEndNewVersions;
  }
}

uint32_t SlotsExamMode::getSlotKhiStartExamAddress() {
  return SlotKhiExamModeBufferStart;
}

uint32_t SlotsExamMode::getSlotKhiEndExamAddress() {
  return SlotKhiExamModeBufferEnd;
}

}
}
