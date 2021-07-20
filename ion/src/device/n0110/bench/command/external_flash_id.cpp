#include "command.h"
#include <drivers/external_flash.h>
#include <poincare/print_int.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void ExternalFlashId(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  constexpr int firstNumberIndex = 18;
  constexpr int numberLength = 3;

  constexpr int bufferSize = firstNumberIndex + numberLength * 3 + 1;
  char response[bufferSize] = {'E', 'X', 'T', 'E', 'R', 'N', 'A', 'L', '_', 'F', 'L', 'A', 'S', 'H', '_', 'I', 'D', '=', 0};

  uint8_t manufacturerID = 0;
  uint8_t memoryType = 0;
  uint8_t capacityType = 0;
  ExternalFlash::JDECid(&manufacturerID, &memoryType, &capacityType);
  Poincare::PrintInt::Right(manufacturerID, response + firstNumberIndex, numberLength);
  Poincare::PrintInt::Right(memoryType, response + firstNumberIndex + numberLength, numberLength);
  Poincare::PrintInt::Right(capacityType, response + firstNumberIndex + 2*numberLength, numberLength);
  response[bufferSize -1] = 0;
  reply(response);
}

}
}
}
}
