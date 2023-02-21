#include <omg/bit_helper.h>
#include <quiz.h>

using namespace OMG;

QUIZ_CASE(omg_bit_helper) {
  quiz_assert(BitHelper::numberOfBitsInType<uint32_t>() == 32);
  quiz_assert(BitHelper::numberOfBitsInType<int8_t>() == 8);
  uint8_t mask = 0b10101111;
  quiz_assert(BitHelper::bitAtIndex(mask, 2) == true);
  quiz_assert(BitHelper::bitAtIndex(mask, 4) == false);
  BitHelper::setBitAtIndex(mask, 4, true);
  quiz_assert(BitHelper::bitAtIndex(mask, 4) == true);
  quiz_assert(BitHelper::countLeadingZeros(0x00F00000) == 8);
  quiz_assert(BitHelper::countTrailingZeros(0x00F00000) == 20);
  quiz_assert(BitHelper::numberOfOnes(0x00F00000) == 4);
  quiz_assert(BitHelper::indexOfMostSignificantBit(0x00F00000) == 23);
  quiz_assert(BitHelper::numberOfBitsToCountUpTo(16) == 4);
}
