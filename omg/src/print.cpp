#include <omg/print.h>

namespace OMG {

int Print::UInt32(Base base, uint32_t integer, LeadingZeros printLeadingZeros,
                  char* buffer, int bufferSize) {
  assert(base != Base::Decimal);
  uint8_t baseValue = static_cast<uint8_t>(base);
  size_t numberOfBitsPerDigit =
      OMG::BitHelper::numberOfBitsIn<uint32_t>() / MaxLengthOfUInt32(base);
  size_t length = printLeadingZeros == LeadingZeros::Keep
                      ? MaxLengthOfUInt32(base)
                      : LengthOfUInt32(base, integer);
  assert(static_cast<int>(length) < bufferSize);
  for (size_t index = 0; index < length; index++) {
    buffer[index] = CharacterForDigit(
        base, (integer >> (length - 1 - index) * numberOfBitsPerDigit) &
                  (baseValue - 1));
  }
  return length;
}

}  // namespace OMG
