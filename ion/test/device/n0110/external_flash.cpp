#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <drivers/config/external_flash.h>
#include <drivers/external_flash.h>
#include "ion/include/ion/timing.h"

// Choose some not too uniform data to program and test the external flash memory with.

static inline uint8_t expected_value_at(uint8_t * ptr) {
  uint32_t address = reinterpret_cast<uint32_t>(ptr) - Ion::Device::ExternalFlash::Config::StartAddress;
  return (address / 0x10000) + (address / 0x100) + address;
  // Example: the value expected at the address 0x123456 is 0x12 + 0x34 + 0x56.
}

static inline uint16_t expected_value_at(uint16_t * ptr) {
  uint8_t * ptr8 = reinterpret_cast<uint8_t *>(ptr);
  return (static_cast<uint16_t>(expected_value_at(ptr8+1)) << 8) | static_cast<uint16_t>(expected_value_at(ptr8));
}

static inline uint32_t expected_value_at(uint32_t * ptr) {
  uint16_t * ptr16 = reinterpret_cast<uint16_t *>(ptr);
  return (static_cast<uint32_t>(expected_value_at(ptr16+1)) << 16) + static_cast<uint32_t>(expected_value_at(ptr16));
}

template <typename T>
static inline void check(volatile T * p, int repeat) {
  for (int i = 0; i < repeat; i++) {
    quiz_assert(*p == expected_value_at(const_cast<T*>(p)));
  }
}

template <typename T>
void test(int accessType, int repeat) {
  uint8_t * start = reinterpret_cast<uint8_t *>(Ion::Device::ExternalFlash::Config::StartAddress);
  uint8_t * end = reinterpret_cast<uint8_t *>(Ion::Device::ExternalFlash::Config::StartAddress + Ion::Device::ExternalFlash::FlashAddressSpaceSize);

  // Forward sequential access
  if (accessType == 0) {
    for (uint8_t * p = start; p <= end-sizeof(T); p++) {
      volatile T * q = reinterpret_cast<T *>(p);
      check(q, repeat);
    }
  }

  // Backward sequential access
  if (accessType == 1) {
    for (uint8_t * p = end - sizeof(T); p >= start; p--) {
      volatile T * q = reinterpret_cast<T *>(p);
      check(q, repeat);
    }
  }

  // Random access
  if (accessType == 2) {
    T * endT = reinterpret_cast<T *>(Ion::Device::ExternalFlash::Config::StartAddress + Ion::Device::ExternalFlash::FlashAddressSpaceSize);
    for (size_t i=0; i<Ion::Device::ExternalFlash::FlashAddressSpaceSize; i++) {
      uint32_t randomAddr = Ion::random() >> (32 - Ion::Device::ExternalFlash::NumberOfAddressBitsInChip);
      volatile T * q = reinterpret_cast<T *>(randomAddr + Ion::Device::ExternalFlash::Config::StartAddress);
      if (q <= endT - 1) {
        check(q, repeat);
      }
    }
  }
}

static size_t uint64ToString(uint64_t n, char buffer[]) {
  size_t len = 0;
  do {
    buffer[len++] = (n % 10) + '0';
  } while ((n /= 10) > 0);
  int i = 0;
  int j = len - 1;
  while (i < j) {
    char c = buffer[i];
    buffer[i++] = buffer[j];
    buffer[j--] = c;
  }
  return len;
}

static void printElapsedTime(uint64_t startTime) {
  char buffer[7+26+2] = " time: ";
  size_t len = uint64ToString((Ion::Timing::millis() - startTime)/1000, buffer+7);
  buffer[7+len] = 's';
  buffer[7+len+1] = 0;
  quiz_print(buffer);
}

QUIZ_CASE(ion_ext_flash_erase) {
#if TEST_EXT_FLASH_REPROGRAM
  uint64_t startTime = Ion::Timing::millis();
  Ion::Device::ExternalFlash::MassErase();
  printElapsedTime(startTime);
#endif
}

QUIZ_CASE(ion_ext_flash_program) {
#if TEST_EXT_FLASH_REPROGRAM
  // Program separately each page of the flash memory
  uint64_t startTime = Ion::Timing::millis();
  for (int page = 0; page < (1<<15); page++) {
    uint8_t buffer[256];
    for (int byte = 0; byte < 256; byte++) {
      buffer[byte] = expected_value_at(reinterpret_cast<uint8_t *>(Ion::Device::ExternalFlash::Config::StartAddress + page * 256 + byte));
    }
    Ion::Device::ExternalFlash::WriteMemory(reinterpret_cast<uint8_t *>(page * 256), buffer, 256);
  }
  printElapsedTime(startTime);
#endif
}

QUIZ_CASE(ion_extflash_read_byte_fwd) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint8_t>(0, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_byte_bck) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint8_t>(1, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_byte_rand) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint8_t>(2, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_half_fwd) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint16_t>(0, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_half_bck) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint16_t>(1, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_half_rand) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint16_t>(2, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_word_fwd) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint32_t>(0, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_word_bck) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint32_t>(1, 1);
  printElapsedTime(startTime);
}

QUIZ_CASE(ion_extflash_read_word_rand) {
  uint64_t startTime = Ion::Timing::millis();
  test<uint32_t>(2, 1);
  printElapsedTime(startTime);
  Ion::Timing::msleep(3000);
}
