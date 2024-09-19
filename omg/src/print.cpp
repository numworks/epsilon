#include <limits.h>
#include <omg/ieee754.h>
#include <omg/print.h>

namespace OMG::Print {

int UInt32(Base base, uint32_t integer, LeadingZeros printLeadingZeros,
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

static int PrintIntInBuffer(uint32_t integer, char* buffer, int bufferLength,
                            bool left) {
  int wantedLength = OMG::IEEE754<double>::exponentBase10((double)integer) + 1;
  assert(wantedLength > 0);
  if (bufferLength == 0 || wantedLength > bufferLength) {
    return wantedLength;
  }

  uint32_t modulo = 10;
  uint32_t startIndexPlusOne = left ? wantedLength : bufferLength;
  for (int i = startIndexPlusOne; i > 0; i--) {
    /* If the integer is written on the right, after all digits are written
     * integer is equal to 0 and the buffer is padded with 0 on the left. */
    uint32_t c = integer % modulo;
    buffer[i - 1] = '0' + c;
    integer = (integer - c) / modulo;
  }
  return wantedLength;
}

int IntLeft(uint32_t integer, char* buffer, int bufferLength) {
  return PrintIntInBuffer(integer, buffer, bufferLength, true);
}

int IntRight(uint32_t integer, char* buffer, int bufferLength) {
  PrintIntInBuffer(integer, buffer, bufferLength, false);
  return bufferLength;
}

uint32_t ParseDecimalInt(const char* text, int maxNumberOfDigits) {
  uint32_t result = 0;
  assert(*text);
  while (maxNumberOfDigits && '0' <= *text && *text <= '9') {
    assert(result < UINT32_MAX / 10 - 1);
    result = 10 * result + (*text - '0');
    text++;
    maxNumberOfDigits--;
  }
  return result;
}

}  // namespace OMG::Print
