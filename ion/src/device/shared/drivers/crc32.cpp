#include <ion.h>
#include <regs/regs.h>

namespace Ion {

using namespace Device::Regs;

uint32_t crc32(const uint32_t * data, size_t length) {
  bool initialCRCEngineState = RCC.AHB1ENR()->getCRCEN();
  RCC.AHB1ENR()->setCRCEN(true);
  CRC.CR()->setRESET(true);

  const uint32_t * end = data + length;
  while (data < end) {
    CRC.DR()->set(*data++);
  }

  uint32_t result = CRC.DR()->get();
  RCC.AHB1ENR()->setCRCEN(initialCRCEngineState);
  return result;
}

}
