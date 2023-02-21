#include <drivers/external_flash.h>
#include <ion.h>
#include <quiz.h>
#include <quiz/stopwatch.h>

#include "external_flash_helper.h"

template <typename T>
static inline void check(volatile T *p, int repeat) {
  for (int i = 0; i < repeat; i++) {
    quiz_assert(*p == expected_value_at(const_cast<T *>(p)));
  }
}

template <typename T>
void test(int accessType, int repeat) {
  uint8_t *start = reinterpret_cast<uint8_t *>(
      Ion::Device::ExternalFlash::Config::StartAddress);
  uint8_t *end = reinterpret_cast<uint8_t *>(
      Ion::Device::ExternalFlash::Config::StartAddress +
      Ion::Device::ExternalFlash::FlashAddressSpaceSize);

  // Forward sequential access
  if (accessType == 0) {
    for (uint8_t *p = start; p <= end - sizeof(T); p++) {
      volatile T *q = reinterpret_cast<T *>(p);
      check(q, repeat);
    }
  }

  // Backward sequential access
  if (accessType == 1) {
    for (uint8_t *p = end - sizeof(T); p >= start; p--) {
      volatile T *q = reinterpret_cast<T *>(p);
      check(q, repeat);
    }
  }

  // Random access
  if (accessType == 2) {
    T *endT = reinterpret_cast<T *>(
        Ion::Device::ExternalFlash::Config::StartAddress +
        Ion::Device::ExternalFlash::FlashAddressSpaceSize);
    for (size_t i = 0; i < Ion::Device::ExternalFlash::FlashAddressSpaceSize;
         i++) {
      uint32_t randomAddr =
          Ion::random() >>
          (32 - Ion::Device::ExternalFlash::NumberOfAddressBitsInChip);
      volatile T *q = reinterpret_cast<T *>(
          randomAddr + Ion::Device::ExternalFlash::Config::StartAddress);
      if (q <= endT - 1) {
        check(q, repeat);
      }
    }
  }
}

QUIZ_CASE(ion_extflash_read_byte_fwd) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint8_t>(0, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_byte_bck) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint8_t>(1, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_byte_rand) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint8_t>(2, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_half_fwd) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint16_t>(0, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_half_bck) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint16_t>(1, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_half_rand) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint16_t>(2, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_word_fwd) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint32_t>(0, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_word_bck) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint32_t>(1, 1);
  quiz_stopwatch_print_lap(startTime);
}

QUIZ_CASE(ion_extflash_read_word_rand) {
  uint64_t startTime = quiz_stopwatch_start();
  test<uint32_t>(2, 1);
  quiz_stopwatch_print_lap(startTime);
  Ion::Timing::msleep(3000);
}
