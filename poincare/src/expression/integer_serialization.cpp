#include <poincare/old/serialization_helper.h>
#include <poincare/print_float.h>

#include "integer.h"
#include "sign.h"

using Poincare::Preferences;
using Poincare::PrintFloat;
using Poincare::SerializationHelper::CodePoint;

namespace Poincare::Internal {

void IntegerHandler::removeZeroAtTheEnd(int minimalNumbersOfDigits,
                                        WorkingBuffer* workingBuffer) {
  /* Remove the zeroes at the end of an integer, respecting the minimum number
   * of digits asked for.
   *
   * For instance :
   *
   * i = 10000
   * i.removeZeroAtTheEnd(2)
   * assert(i==10)
   *
   * i = 1000
   * i.removeZeroAtTheEnd(-1)
   * assert(i==1)
   */

  if (isZero()) {
    return;
  }

  /* If we check the number of digits, we want *i to stay outside of the
   * interval ]-10^numberDigits; 10^numberDigits[. */
  const bool shouldCheckMinimalNumberOfDigits = minimalNumbersOfDigits > 0;
  IntegerHandler minimum =
      // shouldCheckMinimalNumberOfDigits ?
      IntegerHandler((int64_t)std::pow(10.0, minimalNumbersOfDigits - 1));
  // : Integer::Overflow(false);
  IntegerHandler minusMinimum =
      // shouldCheckMinimalNumberOfDigits ?
      IntegerHandler(-(int64_t)std::pow(10.0, minimalNumbersOfDigits - 1));
  // : Integer::Overflow(false);

  IntegerHandler base = IntegerHandler(10);
  DivisionResult<IntegerHandler> d = Udiv(*this, base, workingBuffer);
  while (d.remainder.isZero()) {
    if (shouldCheckMinimalNumberOfDigits &&
        (Compare(d.quotient, minimum) <= 0 &&
         Compare(d.quotient, minusMinimum) >= 0)) {
      break;
    }
    *this = d.quotient;
    d = Udiv(*this, base, workingBuffer);
  }
  // assert(!isOverflow());
}

size_t IntegerHandler::serializeInDecimal(char* buffer, size_t bufferSize,
                                          WorkingBuffer* workingBuffer) const {
  IntegerHandler base(10);
  DivisionResult<IntegerHandler> d = Udiv(*this, base, workingBuffer);

  size_t length = 0;
  if (isZero()) {
    length += CodePoint(buffer + length, bufferSize - length, '0');
  } else if (m_sign == NonStrictSign::Negative) {
    length += CodePoint(buffer + length, bufferSize - length, '-');
  }

  while (!(d.remainder.isZero() && d.quotient.isZero())) {
    char c = OMG::Print::CharacterForDigit(
        OMG::Base::Decimal, d.remainder.isZero() ? 0 : d.remainder.digit(0));
    if (length >= bufferSize - 1) {
      return PrintFloat::ConvertFloatToText<float>(
                 NAN, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength,
                 PrintFloat::k_maxNumberOfSignificantDigits,
                 Preferences::PrintFloatMode::Decimal)
          .CharLength;
    }
    length += CodePoint(buffer + length, bufferSize - length, c);
    d = Udiv(d.quotient, base, workingBuffer);
  }
  assert(length <= bufferSize - 1);
  assert(buffer[length] == 0);

  // Flip the string
  for (int i = m_sign == NonStrictSign::Negative, j = length - 1; i < j;
       i++, j--) {
    char c = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = c;
  }
  return length;
}

size_t IntegerHandler::serialize(char* buffer, size_t bufferSize,
                                 WorkingBuffer* workingBuffer,
                                 OMG::Base base) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }
#if 0
  if (isOverflow()) {
    return PrintFloat::ConvertFloatToText<float>(
               m_negative ? -INFINITY : INFINITY, buffer, bufferSize,
               PrintFloat::k_maxFloatGlyphLength,
               PrintFloat::k_maxNumberOfSignificantDigits,
               Preferences::PrintFloatMode::Decimal)
        .CharLength;
  }
  assert(base == OMG::Base::Decimal);
  switch (base) {
    case OMG::Base::Binary:
      return serializeInBinaryBase(buffer, bufferSize, 'b', OMG::Base::Binary);
    case OMG::Base::Decimal:
      return serializeInDecimal(buffer, bufferSize);
    default:
      assert(base == OMG::Base::Hexadecimal);
      return serializeInBinaryBase(buffer, bufferSize, 'x',
                                   OMG::Base::Hexadecimal);
  }
#endif
  return serializeInDecimal(buffer, bufferSize, workingBuffer);
}

}  // namespace Poincare::Internal
