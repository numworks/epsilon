#include <omg/utf8_helper.h>
#include <poincare/print_float.h>
#include <poincare/sign.h>

#include "integer.h"

using Poincare::Preferences;
using Poincare::PrintFloat;
using UTF8Helper::WriteCodePoint;

namespace Poincare::Internal {

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
void IntegerHandler::removeZeroAtTheEnd(int minimalNumbersOfDigits,
                                        WorkingBuffer* workingBuffer) {
  if (isZero()) {
    return;
  }

  /* If we check the number of digits, we want *i to stay outside of the
   * interval ]-10^numberDigits; 10^numberDigits[. */
  const bool shouldCheckMinimalNumberOfDigits = minimalNumbersOfDigits > 0;

  double minimumValue = std::pow(10.0, minimalNumbersOfDigits - 1);
  // IntegerHandler builder with int64_t is not implemented yet.
  assert(minimumValue <= UINT32_MAX);
  uint32_t minimumValue32 = static_cast<uint32_t>(minimumValue);

  IntegerHandler minimum =
      // !shouldCheckMinimalNumberOfDigits ? Integer::Overflow(false) :
      IntegerHandler(minimumValue32, NonStrictSign::Positive);
  IntegerHandler minusMinimum =
      // !shouldCheckMinimalNumberOfDigits ? Integer::Overflow(false) :
      IntegerHandler(minimumValue32, NonStrictSign::Negative);

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

size_t IntegerHandler::serialize(char* buffer, size_t bufferSize,
                                 WorkingBuffer* workingBuffer) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }

  IntegerHandler base(10);
  DivisionResult<IntegerHandler> d = Udiv(*this, base, workingBuffer);

  size_t length = 0;
  if (isZero()) {
    length += WriteCodePoint(buffer + length, bufferSize - length, '0');
  } else if (m_sign == NonStrictSign::Negative) {
    length += WriteCodePoint(buffer + length, bufferSize - length, '-');
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
    length += WriteCodePoint(buffer + length, bufferSize - length, c);
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

}  // namespace Poincare::Internal
