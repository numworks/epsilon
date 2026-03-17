#include "decimal.h"

#include <omg/utf8_helper.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include <algorithm>
#include <cmath>

#include "integer.h"

namespace Poincare::Internal {

void Decimal::Project(Tree* e) {
  assertValidDecimal(e);
  // dec(x, n) -> 10^(-n)*x
  PatternMatching::MatchReplace(e, KDecimal(KA, KB),
                                KMult(KPow(10_e, KMult(-1_e, KB)), KA));
}

using Poincare::Preferences;
using Poincare::PrintFloat;
using UTF8Helper::WriteCodePoint;

int Decimal::Serialize(const Tree* decimal, char* buffer, int bufferSize,
                       Preferences::PrintFloatMode mode, bool keepMode,
                       PrintFloat::DecimalModeThreshold threshold,
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

  WorkingBuffer workingBuffer;
  uint8_t* const localStart = workingBuffer.localStart();

  /* Step 1: Round mantissa to numberOfSignificantDigits.
   * Decimal(mantissa, storedExp) = mantissa × 10^(-storedExp). */
  const int storedExp = Integer::Handler(decimal->child(1)).to<int>();
  IntegerHandler m = Integer::Handler(unsignedMantissa);
  int nbOfMantissaDigits = m.numberOfBase10DigitsWithoutSign(&workingBuffer);
  int displayExp =
      nbOfMantissaDigits - 1 - storedExp;  // Display exponent: most significant
                                           // digit represents 10^displayExp
  if (nbOfMantissaDigits > numberOfSignificantDigits) {
    int excess = nbOfMantissaDigits - numberOfSignificantDigits;
    IntegerHandler value = 1;
    for (int i = 0; i < excess; i++) {
      value = IntegerHandler::Mult(10, value, &workingBuffer);
      workingBuffer.garbageCollect({&m, &value}, localStart);
    }
    DivisionResult<IntegerHandler> d =
        IntegerHandler::Udiv(m, value, &workingBuffer);
    m = d.quotient;
    IntegerHandler boundary =
        IntegerHandler::Udiv(value, 2, &workingBuffer).quotient;
    if (IntegerHandler::Compare(d.remainder, boundary) >= 0) {
      m = IntegerHandler::Sum(m, IntegerHandler(1), false, &workingBuffer);
      /* If 9999 was rounded to 10000, increment displayExp and drop the extra
       * digit */
      if (m.numberOfBase10DigitsWithoutSign(&workingBuffer) >
          numberOfSignificantDigits) {
        displayExp++;
        m = IntegerHandler::Udiv(m, IntegerHandler(10), &workingBuffer)
                .quotient;
      }
    }
  }

  // Step 2: Engineering zero-padding.
  int exponentForEngineering = 0;
  int minNbOfMantissaDigits = -1;
  bool removeZeroes = true;
  if (mode == Preferences::PrintFloatMode::Engineering) {
    exponentForEngineering =
        PrintFloat::EngineeringExponentFromBase10Exponent(displayExp);
    minNbOfMantissaDigits =
        static_cast<int>(PrintFloat::EngineeringMinimalNumberOfDigits(
            displayExp, exponentForEngineering));
    int zeroesToAdd =
        static_cast<int>(PrintFloat::EngineeringNumberOfZeroesToAdd(
            static_cast<size_t>(minNbOfMantissaDigits),
            m.numberOfBase10DigitsWithoutSign(&workingBuffer)));
    if (zeroesToAdd > 0) {
      for (int i = 0; i < zeroesToAdd; i++) {
        m = IntegerHandler::Mult(m, IntegerHandler(10), &workingBuffer);
        workingBuffer.garbageCollect({&m}, localStart);
      }
      removeZeroes = false;
    }
  }

  // Step 3: Extract digit string.
  /* Remove trailing zeroes created by rounding (e.g. 1999 rounded to 2000
   * should print as 2, not 2.000). */
  if (removeZeroes) {
    m.removeZeroAtTheEnd(minNbOfMantissaDigits, &workingBuffer);
  }

  // Print the sign
  int currentPosition = 0;
  auto addChar = [&buffer, &bufferSize, &currentPosition](char c) {
    if (currentPosition >= bufferSize - 1) {
      return false;
    }
    buffer[currentPosition++] = c;
    return true;
  };
  if (negative) {
    currentPosition += WriteCodePoint(buffer, bufferSize, '-');
    if (currentPosition >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Serialize the mantissa
  assert(
      numberOfSignificantDigits <=
      static_cast<int>(PrintFloat::k_maxNumberOfSignificantDigitsInDecimals));
  char digits[PrintFloat::k_maxNumberOfSignificantDigitsInDecimals + 1];
  int digitsLength =
      static_cast<int>(m.serialize(digits, sizeof(digits), &workingBuffer));
  assert(strcmp(digits, "inf") != 0);
  assert(strcmp(digits, "-inf") != 0);

  // Stop here if m is undef
  if (strcmp(digits, "undef") == 0) {
    currentPosition +=
        strlcpy(buffer + currentPosition, digits, bufferSize - currentPosition);
    return std::min(currentPosition, bufferSize - 1);
  }
  assert(m.numberOfBase10DigitsWithoutSign(&workingBuffer) == digitsLength);

  // Step 4: use scientific, engineering or decimal notation
  /* We force scientific mode if:
   * - the number of digits before the dot (i.e. the exponent) is too big
   *   compared to the number of significant digits (i.e. with 4 significant
   *   digits, 12345 -> 1.235E4 or 12340 -> 1.234E4)
   * - the number would be represented with too many zeros because it is too
   *   small (i.e. 0.00012345 -> 1.2345E-4) */
  bool forceScientificMode =
      displayExp >= numberOfSignificantDigits ||
      std::pow(10., displayExp) <
          PrintFloat::DecimalModeMinimalValue<double>(threshold);
  bool useScientificOrEngineering =
      mode == Preferences::PrintFloatMode::Engineering ||
      mode == Preferences::PrintFloatMode::Scientific ||
      (!keepMode && forceScientificMode);

  // Step 4.1: scientific or engineering
  if (useScientificOrEngineering) {
    /* We should use the UTF8Helper to manipulate chars, but it is clearer to
     * manipulate chars directly, so we just put assumptions on the char size of
     * the code points we manipulate. */
    assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
    int nbDigitsBeforeDot = (mode == Preferences::PrintFloatMode::Engineering)
                                ? minNbOfMantissaDigits
                                : 1;
    if (digitsLength > nbDigitsBeforeDot) {
      for (int i = 0; i < nbDigitsBeforeDot; i++) {
        if (!addChar(digits[i])) return bufferSize - 1;
      }
      if (!addChar('.')) return bufferSize - 1;
      for (int i = nbDigitsBeforeDot; i < digitsLength; i++) {
        if (!addChar(digits[i])) return bufferSize - 1;
      }
    } else {
      for (int i = 0; i < digitsLength; i++) {
        if (!addChar(digits[i])) return bufferSize - 1;
      }
    }
    if (currentPosition >= bufferSize - 1) {
      return bufferSize - 1;
    }
    int displayedExp = (mode == Preferences::PrintFloatMode::Engineering)
                           ? exponentForEngineering
                           : displayExp;
    if (displayedExp == 0) {
      buffer[currentPosition] = 0;
      return currentPosition;
    }
    currentPosition +=
        WriteCodePoint(buffer + currentPosition, bufferSize - currentPosition,
                       UCodePointLatinLetterSmallCapitalE);
    if (currentPosition >= bufferSize - 1) {
      return bufferSize - 1;
    }
    if (mode == Preferences::PrintFloatMode::Engineering) {
      currentPosition += static_cast<int>(
          IntegerHandler(exponentForEngineering)
              .serialize(buffer + currentPosition, bufferSize - currentPosition,
                         &workingBuffer));
    } else {
      currentPosition += static_cast<int>(
          IntegerHandler(displayExp)
              .serialize(buffer + currentPosition, bufferSize - currentPosition,
                         &workingBuffer));
    }
    return currentPosition;
  }

  // Step 4.2: decimal
  assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
  assert(UTF8Decoder::CharSizeOfCodePoint('0') == 1);
  if (displayExp < 0) {
    /* Case 1: "0.000...0<digits>"
     * displayExp is the exponent of the leading digit, so we need
     * (-displayExp-1) zeros between the decimal point and the first digit. */
    if (!addChar('0')) return bufferSize - 1;
    if (!addChar('.')) return bufferSize - 1;
    for (int i = 0; i < -displayExp - 1; i++) {
      if (!addChar('0')) return bufferSize - 1;
    }
    for (int i = 0; i < digitsLength; i++) {
      if (!addChar(digits[i])) return bufferSize - 1;
    }
  } else if (displayExp < digitsLength - 1) {
    /* Case 2: "DDD.DDD"
     * decimal point sits after the (displayExp+1)-th digit. */
    for (int i = 0; i <= displayExp; i++) {
      if (!addChar(digits[i])) return bufferSize - 1;
    }
    if (!addChar('.')) return bufferSize - 1;
    for (int i = displayExp + 1; i < digitsLength; i++) {
      if (!addChar(digits[i])) return bufferSize - 1;
    }
  } else {
    /* Case 3: "DDD000"
     * integer with (displayExp - digitsLength + 1) trailing zeros. */
    for (int i = 0; i < digitsLength; i++) {
      if (!addChar(digits[i])) return bufferSize - 1;
    }
    for (int i = digitsLength - 1; i < displayExp; i++) {
      if (!addChar('0')) return bufferSize - 1;
    }
  }

  buffer[currentPosition] = 0;
  return currentPosition;
}

}  // namespace Poincare::Internal
