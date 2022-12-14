#include <omg/print.h>

namespace OMG {

int Print::UInt32(Base base, uint32_t integer, char * buffer, int bufferSize) {
  // TODO: implementation for Base::Decimal
  uint8_t baseValue = static_cast<uint8_t>(base);
  size_t numberOfBitsPerDigit = OMG::BitHelper::numberOfBitsInType<uint32_t>() / LengthOfUInt32(base);
  assert(LengthOfUInt32(base) < bufferSize);
  for (size_t index = 0; index <  LengthOfUInt32(base); index++) {
    buffer[index] = CharacterForDigit(base, (integer >> (LengthOfUInt32(base) - 1 - index) * numberOfBitsPerDigit) & (baseValue - 1));
  }
  return LengthOfUInt32(base);
}

}
