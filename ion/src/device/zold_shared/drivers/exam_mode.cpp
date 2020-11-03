#include <ion/exam_mode_unprivileged.h>
#include <drivers/config/exam_mode.h>
#include <drivers/svcall.h>
#include "exam_mode.h"
#include "flash.h"
#include <assert.h>

namespace Ion {
namespace ExamMode {

/* TODO EMILIE these pointers should be retrieved at linking or this code should
 *  be done in External flash (only the Ion::Device::Flash::WriteMemory should
 *  be done in internal flash)... However, we need FetchExamMode in Updatable bootloader and in main Epsilon. The info of where is the exam mode should be shared between both in a smart way? */

constexpr static const uint32_t _exam_mode_buffer_start_address = 0x9000c028;
constexpr static const uint32_t _exam_mode_buffer_end_address = 0x9000d028;

// TODO EMILIE Duplicata - see previous comment TODO EMILIE
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

// TODO EMILIE Duplicata - see previous comment TODO EMILIE
uint8_t * SignificantPersistedByteAddress() {
  uint32_t * persitence_start_32 = (uint32_t *)_exam_mode_buffer_start_address;
  uint32_t * persitence_end_32 = (uint32_t *)_exam_mode_buffer_end_address;
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
    assert(Ion::Device::Flash::SectorAtAddress(_exam_mode_buffer_start_address) >= 0);
    Ion::Device::Flash::EraseSector(Ion::Device::Flash::SectorAtAddress(_exam_mode_buffer_start_address));
    return (uint8_t *)_exam_mode_buffer_start_address;
  }

  return persitence_start_8;
}

uint8_t FetchExamMode() {
  uint8_t * readingAddress = SignificantExamModeAddress();
  // Count the number of 0[3] before reading address
  uint32_t nbOfZerosBefore = ((readingAddress - (uint8_t *)_exam_mode_buffer_start_address) * numberOfBitsInByte) % 3;
  // Count the number of 0[3] at reading address
  size_t numberOfLeading0 = (numberOfBitsInByte - numberOfBitsAfterLeadingZeroes(*readingAddress)) % 3;
  return (nbOfZerosBefore + numberOfLeading0) % 3;
}

}
}
