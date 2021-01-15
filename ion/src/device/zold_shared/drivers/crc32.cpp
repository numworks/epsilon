#include <ion.h>
#include <regs/regs.h>

namespace Ion {

using namespace Device::Regs;

uint32_t crc32Byte(const uint8_t * data, size_t length) {
  uint32_t result = 0;
  bool initialCRCEngineState = RCC.AHB1ENR()->getCRCEN();
  RCC.AHB1ENR()->setCRCEN(true);
  CRC.CR()->setRESET(true);

  const uint8_t * end = data + length;
  int wordSize = (length * sizeof(uint8_t)) / sizeof(uint32_t);

  // Make as many word accesses as possible
  for (int i = 0; i < wordSize; i++) {
    CRC.DR_WordAccess()->set(*((uint32_t *)data));
    data += 4;
  }

#if REGS_CRC_CONFIG_BYTE_ACCESS
  // Scan the remaining data with byte accesses
  while (data < end) {
    CRC.DR_ByteAccess()->set(*data++);
  }
  result = CRC.DR_WordAccess()->get();
#else
  result = CRC.DR_WordAccess()->get();
  while (data < end) {
    result = Ion::crc32EatByte(result, *data++);
  }
#endif

  RCC.AHB1ENR()->setCRCEN(initialCRCEngineState);
  return result;
}

uint32_t crc32Word(const uint32_t * data, size_t length) {
  return crc32Byte((const uint8_t *)data, length * (sizeof(uint32_t)/sizeof(uint8_t)));
}

}
