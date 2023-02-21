#ifndef ION_STORAGE_HELPER_H
#define ION_STORAGE_HELPER_H

namespace Ion {

namespace Storage {
// emscripten read and writes must be aligned.
class StorageHelper {
 public:
  static uint16_t unalignedShort(char *address) {
#if __EMSCRIPTEN__
    uint8_t f1 = *(address);
    uint8_t f2 = *(address + 1);
    uint16_t f = (uint16_t)f1 + (((uint16_t)f2) << 8);
    return f;
#else
    return *(uint16_t *)address;
#endif
  }
  static void writeUnalignedShort(uint16_t value, char *address) {
#if __EMSCRIPTEN__
    *((uint8_t *)address) = (uint8_t)(value & ((1 << 8) - 1));
    *((uint8_t *)address + 1) = (uint8_t)(value >> 8);
#else
    *((uint16_t *)address) = value;
#endif
  }
};

}  // namespace Storage

}  // namespace Ion

#endif
