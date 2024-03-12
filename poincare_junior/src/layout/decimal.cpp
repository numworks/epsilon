#include "decimal.h"

namespace PoincareJ {

int DecimalNode::convertToText(char *buffer, int bufferSize,
                               Preferences::PrintFloatMode mode,
                               int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  if (bufferSize == 1) {
    buffer[0] = 0;
    return 0;
  }
  if (unsignedMantissa().isZero()) {
    // This already writes the null terminating char
    return SerializationHelper::CodePoint(buffer, bufferSize, '0');
  }

  // Compute the exponent
  int exponent = m_exponent;

  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  char tempBuffer[PrintFloat::k_maxNumberOfSignificantDigits + 1];
  Integer m = unsignedMantissa();
  int numberOfDigitsInMantissa = Integer::NumberOfBase10DigitsWithoutSign(m);
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    IntegerDivision d = Integer::Division(
        m, Integer((int64_t)std::pow(
               10.0, numberOfDigitsInMantissa - numberOfSignificantDigits)));
    m = d.quotient;
    int64_t boundary = 5. * std::pow(10., numberOfDigitsInMantissa -
                                              numberOfSignificantDigits - 1);
    if (Integer::NaturalOrder(d.remainder, Integer(boundary)) >= 0) {
      m = Integer::Addition(m, Integer(1));
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (Integer::NumberOfBase10DigitsWithoutSign(m) >
          numberOfSignificantDigits) {
        exponent++;
        m = Integer::Division(m, Integer(10)).quotient;
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
            Integer::NumberOfBase10DigitsWithoutSign(m));
    if (numberOfZeroesToAddForEngineering > 0) {
      for (int i = 0; i < numberOfZeroesToAddForEngineering; i++) {
        m = Integer::Multiplication(m, Integer(10));
      }
      removeZeroes = false;
    }
  }

  /* Remove the final zeroes, that already existed or were created due to
   * rounding. For example 1.999 with 3 significant digits: the mantissa 1999 is
   * rounded to 2000. To avoid printing 2.000, we removeZeroAtTheEnd here. */
  if (removeZeroes) {
    removeZeroAtTheEnd(&m, minimalNumberOfMantissaDigits);
  }

  // Print the sign
  int currentChar = 0;
  if (m_negative) {
    assert(UTF8Decoder::CharSizeOfCodePoint('-') == 1);
    currentChar += SerializationHelper::CodePoint(buffer, bufferSize, '-');
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Serialize the mantissa
  int mantissaLength =
      m.serialize(tempBuffer, PrintFloat::k_maxNumberOfSignificantDigits + 1);

  // Assert that m is not +/-inf
  assert(strcmp(tempBuffer, Infinity::Name(false)) != 0);
  assert(strcmp(tempBuffer, Infinity::Name(true)) != 0);

  // Stop here if m is undef
  if (strcmp(tempBuffer, Undefined::Name()) == 0) {
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
         Integer::NumberOfBase10DigitsWithoutSign(m) >
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
    currentChar += SerializationHelper::CodePoint(
        buffer + currentChar, bufferSize - currentChar,
        UCodePointLatinLetterSmallCapitalE);
    if (currentChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    if (mode == Preferences::PrintFloatMode::Engineering) {
      currentChar +=
          Integer(exponentForEngineeringNotation)
              .serialize(buffer + currentChar, bufferSize - currentChar);
    } else {
      currentChar += Integer(exponent).serialize(buffer + currentChar,
                                                 bufferSize - currentChar);
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
    int decimalMarkerPosition = m_negative ? exponent + 1 : exponent;
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
    int endMarkerPosition = m_negative ? exponent + 1 : exponent;
    for (int i = currentChar - 1; i < endMarkerPosition; i++) {
      currentChar += SerializationHelper::CodePoint(
          buffer + currentChar, bufferSize - currentChar, '0');
      if (currentChar + 1 >= bufferSize - 1) {
        return bufferSize - 1;
      }
    }
  }
  buffer[currentChar] = 0;
  return currentChar;
}

}  // namespace PoincareJ
