#include <drivers/config/external_flash.h>

// Choose some not too uniform data to program and test the external flash
// memory with.

inline uint8_t expected_value_at(uint8_t *ptr) {
  uint32_t address = reinterpret_cast<uint32_t>(ptr) -
                     Ion::Device::ExternalFlash::Config::StartAddress;
  return (address / 0x10000) + (address / 0x100) + address;
  // Example: the value expected at the address 0x123456 is 0x12 + 0x34 + 0x56.
}

inline uint16_t expected_value_at(uint16_t *ptr) {
  uint8_t *ptr8 = reinterpret_cast<uint8_t *>(ptr);
  return (static_cast<uint16_t>(expected_value_at(ptr8 + 1)) << 8) |
         static_cast<uint16_t>(expected_value_at(ptr8));
}

inline uint32_t expected_value_at(uint32_t *ptr) {
  uint16_t *ptr16 = reinterpret_cast<uint16_t *>(ptr);
  return (static_cast<uint32_t>(expected_value_at(ptr16 + 1)) << 16) +
         static_cast<uint32_t>(expected_value_at(ptr16));
}
