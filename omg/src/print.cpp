#include <ion/bit_helper.h>
#include <omg/print.h>

namespace OMG {

int Print::Integer(Base base, uint32_t integer, char * buffer, int bufferSize) {
  // TODO: implementation for Base::Decimal
  uint8_t baseValue = static_cast<uint8_t>(base);
  size_t numberOfDigits =  Ion::BitHelper::numberOfBitsInType<uint32_t>() / Ion::BitHelper::numberOfBitsToCountUpTo(baseValue);
  size_t numberOfBitsPerDigit = Ion::BitHelper::numberOfBitsInType<uint32_t>() / numberOfDigits;
  assert(numberOfDigits < bufferSize);
  for (size_t index = 0; index < numberOfDigits; index++) {
    buffer[index] = CharacterForDigit(base, (integer >> (numberOfDigits - 1 - index) * numberOfBitsPerDigit) & (baseValue - 1));
  }
  return numberOfDigits;
}

}
