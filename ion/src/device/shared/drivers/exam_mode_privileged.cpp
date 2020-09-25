#include "exam_mode_privileged.h"
#include "flash.h"
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExamMode {

void externalFlashWrite(uint8_t * writingAddress, uint8_t newValue[]) {
  // Write the value in flash
  /* As the number of changed bits is capped by 2, if *writingAddress has more
   * than one remaining 1 bit, we know we toggle bits only in the first byte of
   * newValue. We can settle for writing one byte instead of two. */
  size_t writtenFlash = *writingAddress == 1 ? sizeof(uint16_t) : sizeof(uint8_t);
  /* Avoid writing out of sector */
  //assert(writingAddress < (uint8_t *)&_exam_mode_buffer_end - 1 || (writingAddress == (uint8_t *)&_exam_mode_buffer_end - 1 && writtenFlash == 1));
  Ion::Device::Flash::WriteMemory(writingAddress, newValue, writtenFlash);
}

}
}
}
