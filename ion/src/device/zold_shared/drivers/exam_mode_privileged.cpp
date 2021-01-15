#include "exam_mode_privileged.h"
#include "exam_mode.h"
#include "flash.h"
#include <drivers/cache.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExamMode {

/* TODO EMILIE we rather do this in unprivileged mode... */
using namespace Ion::ExamMode;

void ToggleExamMode() {
  uint8_t * writingAddress = SignificantExamModeAddress();
  assert(*writingAddress != 0);
  size_t nbOfTargetedOnes = numberOfBitsAfterLeadingZeroes(*writingAddress);

  // Compute the new value with delta bits switched to 0.
  /* We write in 2 bytes instead of 1, in case there was only one bit
   * left to 1 in writingAddress. */
  nbOfTargetedOnes += numberOfBitsInByte;
  nbOfTargetedOnes -= 1;
  constexpr size_t newValueSize = sizeof(uint16_t)/sizeof(uint8_t);
  uint8_t newValue[newValueSize];
  if (nbOfTargetedOnes > numberOfBitsInByte) {
    size_t nbOfTargetedOnesInFirstByte = nbOfTargetedOnes - numberOfBitsInByte;
    assert(nbOfTargetedOnesInFirstByte <= numberOfBitsInByte);
    newValue[0] = ((uint16_t)1 << nbOfTargetedOnesInFirstByte) - 1;
    newValue[1] = 0xFF;
  } else {
    assert(nbOfTargetedOnes <= numberOfBitsInByte);
    newValue[0] = 0;
    newValue[1] = ((uint16_t)1 << nbOfTargetedOnes) - 1;
  }

  // Write the value in flash
  /* As the number of changed bits is capped by 2, if *writingAddress has more
   * than one remaining 1 bit, we know we toggle bits only in the first byte of
   * newValue. We can settle for writing one byte instead of two. */
  size_t writtenFlash = *writingAddress == 1 ? sizeof(uint16_t) : sizeof(uint8_t);
  /* Avoid writing out of sector */
  //assert(writingAddress < _exam_mode_buffer_end_address - 1 || (writingAddress == _exam_mode_buffer_end_address - 1 && writtenFlash == 1));
  Ion::Device::Flash::WriteMemory(writingAddress, newValue, writtenFlash);
  Ion::Device::Cache::invalidateDCache();
}

}
}
}
