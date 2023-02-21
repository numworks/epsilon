#include <drivers/external_flash.h>
#include <quiz.h>
#include <quiz/stopwatch.h>

#include "external_flash_helper.h"

// Choose some not too uniform data to program the external flash memory with.

QUIZ_CASE(ion_ext_flash_erase) {
  uint64_t startTime = quiz_stopwatch_start();
  Ion::Device::ExternalFlash::MassErase();
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_ext_flash_program) {
  // Program separately each page of the flash memory
  uint64_t startTime = quiz_stopwatch_start();
  for (int page = 0; page < (1 << 15); page++) {
    uint8_t buffer[256];
    for (int byte = 0; byte < 256; byte++) {
      buffer[byte] = expected_value_at(reinterpret_cast<uint8_t *>(
          Ion::Device::ExternalFlash::Config::StartAddress + page * 256 +
          byte));
    }
    Ion::Device::ExternalFlash::WriteMemory(
        reinterpret_cast<uint8_t *>(page * 256), buffer, 256);
  }
  quiz_stopwatch_print_lap(startTime);
}
