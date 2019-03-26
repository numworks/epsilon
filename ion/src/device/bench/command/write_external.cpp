#include "command.h"
#include <ion.h>
#include <ion/src/device/shared/drivers/external_flash.h>
#include <kandinsky.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

/* Input must be of the form ??????:data, ?????? being the size of the data in
 * hex */

#if 1
void WriteExternal(const char * input) {
  // Check the input format
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  const char * data = "0987654321098764321098765432109876543210987654321";
  uint8_t * externalFlashStart = reinterpret_cast<uint8_t *>(0x90000000);
  int dataSize = 10;

  // Copy the data in the external flash
  Ion::Device::ExternalFlash::EraseSector(0);
  Ion::Device::ExternalFlash::WriteMemory(
      reinterpret_cast<uint8_t *>(0),
      reinterpret_cast<const uint8_t *>(data) ,
      dataSize);

  // Check it was correctly copied
  for (int i = 0; i < dataSize; i++) {
    if (*(externalFlashStart + i) != data[i]) {
      reply(sKO);
      return;
    }
  }

  Ion::Device::ExternalFlash::EraseSector(0);
  Ion::Device::ExternalFlash::WriteMemory(
      reinterpret_cast<uint8_t *>(0),
      reinterpret_cast<const uint8_t *>(data),
      dataSize);

  // Check it was correctly copied
  for (int i = 0; i < dataSize; i++) {
    if (*(externalFlashStart + i) != data[i]) {
      reply(sKO);
      return;
    }
  }
  reply(sOK);
}


#else
void WriteExternal(const char * input) {
  // Check the input format
  if (input == nullptr || input[0] != '0' || input[1] != 'x') {
    reply(sSyntaxError);
    return;
  }
  constexpr int sizeStart = 2;
  constexpr int dataSizeLength = 6;
  for (int i = sizeStart; i < sizeStart + dataSizeLength; i++) {
    if (!isHex(input[i])) {
      reply(sSyntaxError);
      return;
    }
  }
  if (input[sizeStart + dataSizeLength] != ':') {
    reply(sSyntaxError);
    return;
  }

  // Get the data size
  char dataSizeBuffer[dataSizeLength + 1];
  for (int i = 0; i < dataSizeLength; i++) {
    dataSizeBuffer[i] = input[sizeStart + i];
  }
  dataSizeBuffer[dataSizeLength] = 0;
  uint32_t dataSize = hexNumber(dataSizeBuffer);
  int dataIndex = sizeStart + dataSizeLength + 1;

  uint8_t * externalFlashStart = reinterpret_cast<uint8_t *>(0x90000000); // TODO reinterpret_cast<uint8_t *>(Ion::Device::ExternalFlash::QSPIBaseAddress)

  // Copy the data in the external flash
  uint32_t writeOffset = 0x00000000;
  // Erase the flash
  int firstSector = Ion::Device::ExternalFlash::SectorAtAddress(writeOffset);
  int lastSector = Ion::Device::ExternalFlash::SectorAtAddress(writeOffset + dataSize);
  assert(firstSector <= lastSector);
  for (int i = firstSector; i <= lastSector; i++) {
    Ion::Device::ExternalFlash::EraseSector(i);
  }
  // Write
  Ion::Device::ExternalFlash::WriteMemory(
      reinterpret_cast<uint8_t *>(writeOffset),
      reinterpret_cast<const uint8_t *>(input + dataIndex),
      dataSize);

  // Check it was correctly copied
  for (uint32_t i = 0; i < dataSize; i++) {
    if (*(externalFlashStart + writeOffset + i) != input[dataIndex+i]) {
      reply(sKO);
      return;
    }
  }
  reply(sOK);
}
#endif

}
}
}
}
