#include "decimal.h"

#include <omg/utf8_helper.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack.h>

#include <algorithm>

#include "integer.h"

namespace Poincare::Internal {

void Decimal::Project(Tree* e) {
  assertValidDecimal(e);
  // dec(x, n) -> 10^(-n)*x
  TreeRef mult = SharedTreeStack->pushMult(1);
  SharedTreeStack->pushPow();
  Integer::Push(10);
  SharedTreeStack->pushMult(2);
  (-1_e)->cloneTree();
  e->child(1)->detachTree();
  e->moveTreeOverNode(mult);
  NAry::SetNumberOfChildren(e, 2);
}

using Poincare::Preferences;
using Poincare::PrintFloat;
using UTF8Helper::WriteCodePoint;

int Decimal::Serialize(const Tree* decimal, char* buffer, int bufferSize,
                       Preferences::PrintFloatMode mode,
                       int numberOfSignificantDigits) {
  assert(decimal->isDecimal() ||
         (decimal->isOpposite() && decimal->child(0)->isDecimal()));
  bool negative = decimal->isOpposite();
  if (negative) {
    decimal = decimal->child(0);
  }
  const Tree* unsignedMantissa = decimal->child(0);

  if (bufferSize == 0) {
    return -1;
  }
  if (bufferSize == 1) {
    buffer[0] = 0;
    return 0;
  }
  if (unsignedMantissa->isZero()) {
    // This already writes the null terminating char
    return WriteCodePoint(buffer, bufferSize, '0');
  }

  // Compute the exponent
  int exponent = Integer::Handler(decimal->child(1)).to<int>();

  WorkingBuffer workingBuffer;
  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  char tempBuffer[PrintFloat::k_maxNumberOfSignificantDigits + 1];
  IntegerHandler m = Integer::Handler(unsignedMantissa);
  int numberOfDigitsInMantissa =
      m.numberOfBase10DigitsWithoutSign(&workingBuffer);
  exponent = numberOfDigitsInMantissa - 1 - exponent;
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    double value =
        std::pow(10.0, numberOfDigitsInMantissa - numberOfSignificantDigits);
    // IntegerHandler builder is not implemented on int64_t
    assert(value <= UINT32_MAX);
    uint32_t value32 = static_cast<uint32_t>(value);
    DivisionResult<IntegerHandler> d =
        IntegerHandler::Udiv(m, IntegerHandler(value32), &workingBuffer);
    m = d.quotient;
    uint32_t boundary = value32 / 2;
    if (IntegerHandler::Compare(d.remainder, IntegerHandler(boundary)) >= 0) {
      m = IntegerHandler::Sum(m, IntegerHandler(1), false, &workingBuffer);
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (m.numberOfBase10DigitsWithoutSign(&workingBuffer) >
          numberOfSignificantDigits) {
        exponent++;
        m = IntegerHandler::Udiv(m, IntegerHandler(10), &workingBuffer)
                .quotient;
      }
    }
  }
  int exponentForEngineeringNotation = 0;
  int minimalNumberOfMantissaDigits = -1;
  bool removeZeroes = true;
  if (mode == Preferences::PrintFloatMode::Engineering) {
    exponentForEngineeringNotation =
        PrintFloat::EngineeringExponentFromBase10Exponent(exponent);
    minimalNumberOfMantissaDigits =
        PrintFloat::EngineeringMinimalNumberOfDigits(
            exponent, exponentForEngineeringNotation);
    int numberOfZeroesToAddForEngineering =
        PrintFloat::EngineeringNumberOfZeroesToAdd(
            minimalNumberOfMantissaDigits,
            m.numberOfBase10DigitsWithoutSign(&workingBuffer));
    if (numberOfZeroesToAddForEngineering > 0) {
      for (int i = 0; i < numberOfZeroesToAddForEngineering; i++) {
        m = IntegerHandler::Mult(m, IntegerHandler(10), &workingBuffer);
      }
      removeZeroes = false;
    }
  }

  /* Remove the final zeroes, that already existed or were created due to
   * rounding. For example 1.999 with 3 significant digits: the mantissa 1999 is
   * rounded to 2000. To avoid printing 2.000, we removeZeroAtTheEnd here. */
  if (removeZeroes) {
    m.removeZeroAtTheEnd(minimalNumberOfMantissaDigits, &workingBuffer);
  }

  // Print the sign
  int currentChar = 0;
  if (negative) {
    currentChar += WriteCodePoint(buffer, bufferSize, '-');
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Serialize the mantissa
  int mantissaLength =
      m.serialize(tempBuffer, PrintFloat::k_maxNumberOfSignificantDigits + 1,
                  &workingBuffer);

  // Assert that m is not +/-inf
  assert(strcmp(tempBuffer, "inf") != 0);
  assert(strcmp(tempBuffer, "-inf") != 0);

  // Stop here if m is undef
  if (strcmp(tempBuffer, "undef") == 0) {
    currentChar +=
        strlcpy(buffer + currentChar, tempBuffer, bufferSize - currentChar);
    return std::min(currentChar, bufferSize - 1);
  }

  /* We force scientific mode if the number of digits before the dot is superior
   * to the number of significant digits (ie with 4 significant digits,
   * 12345 -> 1.235E4 or 12340 -> 1.234E4). */
  bool forceScientificMode =
      mode != Preferences::PrintFloatMode::Engineering &&
      (mode == Preferences::PrintFloatMode::Scientific ||
       exponent >= numberOfSignificantDigits ||
       std::pow(10., exponent) < PrintFloat::DecimalModeMinimalValue<double>());
  int numberOfRequiredDigits = (mode == Preferences::PrintFloatMode::Decimal &&
                                !forceScientificMode && exponent >= 0)
                                   ? std::max(mantissaLength, exponent)
                                   : mantissaLength;

  /* Case 1: Engineering and Scientific mode. Three cases:
   * - the user chooses the scientific mode
   * - the exponent is too big compared to the number of significant digits, so
   *   we force the scientific mode to avoid inventing digits
   * - the number would be too long if we print it as a natural decimal */
  if (mode == Preferences::PrintFloatMode::Engineering ||
      numberOfRequiredDigits > PrintFloat::k_maxNumberOfSignificantDigits ||
      forceScientificMode) {
    if (mantissaLength > 1 &&
        (mode != Preferences::PrintFloatMode::Engineering ||
         m.numberOfBase10DigitsWithoutSign(&workingBuffer) >
             minimalNumberOfMantissaDigits)) {
      /* Forward one or more chars: _
       * Write the mantissa _23456
       * Copy the most significant digits on the forwarded chars: 223456
       * Write the dot : 2.3456
       *
       * We should use the UTF8Helper to manipulate chars, but it is clearer to
       * manipulate chars directly, so we just put assumptions on the char size
       * of the code points we manipuate. */
      assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
      currentChar++;
      if (currentChar >= bufferSize - 1) {
        return bufferSize - 1;
      }
      int numberOfCharsToShift =
          (mode == Preferences::PrintFloatMode::Engineering
               ? minimalNumberOfMantissaDigits
               : 1);
      int decimalMarkerPosition = currentChar + numberOfCharsToShift - 1;
      currentChar +=
          strlcpy(buffer + currentChar, tempBuffer, bufferSize - currentChar);
      if (currentChar >= bufferSize - 1) {
        return bufferSize - 1;
      }
      assert(UTF8Decoder::CharSizeOfCodePoint(buffer[decimalMarkerPosition]) ==
             1);
      for (int i = 0; i < numberOfCharsToShift; i++) {
        buffer[i + decimalMarkerPosition - numberOfCharsToShift] =
            tempBuffer[i];
      }
      if (decimalMarkerPosition >= bufferSize - 1) {
        return bufferSize - 1;
      }
      buffer[decimalMarkerPosition] = '.';
    } else {
      currentChar +=
          strlcpy(buffer + currentChar, tempBuffer, bufferSize - currentChar);
    }
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    if ((mode == Preferences::PrintFloatMode::Engineering &&
         exponentForEngineeringNotation == 0) ||
        exponent == 0) {
      return currentChar;
    }
    currentChar +=
        WriteCodePoint(buffer + currentChar, bufferSize - currentChar,
                       UCodePointLatinLetterSmallCapitalE);
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    if (mode == Preferences::PrintFloatMode::Engineering) {
      currentChar += IntegerHandler(exponentForEngineeringNotation)
                         .serialize(buffer + currentChar,
                                    bufferSize - currentChar, &workingBuffer);
    } else {
      currentChar += IntegerHandler(exponent).serialize(
          buffer + currentChar, bufferSize - currentChar, &workingBuffer);
    }
    return currentChar;
  }
  // Case 3: Decimal mode
  assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
  assert(UTF8Decoder::CharSizeOfCodePoint('0') == 1);
  int deltaCharMantissa = exponent < 0 ? -exponent + 1 : 0;
  strlcpy(buffer + currentChar + deltaCharMantissa, tempBuffer,
          std::max(0, bufferSize - deltaCharMantissa - currentChar));
  if (exponent < 0) {
    for (int i = 0; i <= -exponent; i++) {
      buffer[currentChar++] = i == 1 ? '.' : '0';
      if (currentChar >= bufferSize - 1) {
        return bufferSize - 1;
      }
    }
  }
  currentChar += mantissaLength;
  if (currentChar >= bufferSize - 1) {
    return bufferSize - 1;
  }  // Check if strlcpy returned prematuraly
  if (exponent >= 0 && exponent < mantissaLength - 1) {
    if (currentChar + 1 >= bufferSize - 1) {
      return bufferSize - 1;
    }
    int decimalMarkerPosition = negative ? exponent + 1 : exponent;
    for (int i = currentChar - 1; i > decimalMarkerPosition; i--) {
      buffer[i + 1] = buffer[i];
    }
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
    buffer[decimalMarkerPosition + 1] = '.';
    currentChar++;
  }
  if (currentChar + 1 >= bufferSize - 1) {
    return bufferSize - 1;
  }
  if (exponent >= 0 && exponent > mantissaLength - 1) {
    int endMarkerPosition = negative ? exponent + 1 : exponent;
    for (int i = currentChar - 1; i < endMarkerPosition; i++) {
      currentChar +=
          WriteCodePoint(buffer + currentChar, bufferSize - currentChar, '0');
      if (currentChar + 1 >= bufferSize - 1) {
        return bufferSize - 1;
      }
    }
  }
  buffer[currentChar] = 0;
  return currentChar;
}

}  // namespace Poincare::Internal
