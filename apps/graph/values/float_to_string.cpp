#include "float_to_string.h"

void FloatToString::printBase10IntegerWithDecimalMarker(char * buffer, int bufferLength,  int i, int decimalMarkerPosition) {
  /* The decimal marker position is always preceded by a char, thus, it is never
   * in first position. When called by convertFloatToText, the buffer length is
   * always > 0 as we asserted a minimal number of available chars. */
  assert(bufferLength > 0 && decimalMarkerPosition != 0);
  int endChar = bufferLength - 1, startChar = 0;
  int dividend = i, digit = 0, quotien = 0;
  if (i < 0) {
    buffer[startChar++] = '-';
    dividend = -i;
    decimalMarkerPosition += 1;
  }
  /* This loop acts correctly as we asserted the endChar >= 0 and
   * decimalMarkerPosition != 0 */
  do {
    if (endChar == decimalMarkerPosition) {
      buffer[endChar--] = '.';
    }
    quotien = dividend/10;
    digit = dividend - quotien*10;
    buffer[endChar--] = '0'+digit;
    dividend = quotien;
  }  while (endChar >= startChar);
}

void FloatToString::convertFloatToText(float f, char * buffer, int maxNumberOfChar,
    int numberOfDigitsInMantissa, Mode mode) {
  /* We here assert that the buffer is long enough to display with the right
   * number of digits in the mantissa. If numberOfDigitsInMantissa = 7, the
   * worst case has the form -1.999999e-38 (7+6+1 char). */
  assert(maxNumberOfChar > 6 + numberOfDigitsInMantissa);

  //if (isinf(f)) {
  float maximalFloat = 3.4f*powf(10, 38);
  if (f > maximalFloat || f < -maximalFloat) {
    buffer[0] = 'N';
    buffer[1] = 'a';
    buffer[2] = 'N';
    buffer[3] = 0;
    return;
  }

  float logBase10 = f != 0.0f ? log10f(fabsf(f)) : 0;
  int exponentInBase10 = logBase10;
  if ((int)f == 0 and logBase10 != exponentInBase10) {
    /* For floats < 0, the exponent in base 10 is the inferior integer part of
     * log10(float). We thus decrement the exponent for float < 0 whose exponent
     * is not an integer. */
    exponentInBase10--;
  }

  int numberOfCharExponent = exponentInBase10 != 0 ? log10f(fabsf((float)exponentInBase10)) + 1 : 1;
  if (exponentInBase10 < 0){
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
  }

  /* Future optimisation, if mode > 0, find the position of decimalMarker and
   * decide whether to display the exponent. */

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfDigitsInMantissa + 1;
  int availableCharsForMantissaWithSign = f >= 0 ? availableCharsForMantissaWithoutSign : availableCharsForMantissaWithoutSign + 1;

  // Compute mantissa
  /* The number of digits in an integer is capped because the maximal integer is
   * 2^31 - 1. As our mantissa is an integer, we assert that we stay beyond this
   * threshold during computation. */
  int numberMaximalOfCharsInInteger = log10f(powf(2, 31));
  assert(availableCharsForMantissaWithoutSign - 1 < numberMaximalOfCharsInInteger);
  int mantissa = f * powf(10, availableCharsForMantissaWithoutSign - exponentInBase10 - 2);

  // Supress the 0 on the right side of the mantissa
  int dividend = fabsf((float)mantissa);
  int quotien = dividend/10;
  int digit = dividend - quotien*10;
  while (digit == 0 && availableCharsForMantissaWithSign > 2) {
    mantissa = mantissa/10;
    availableCharsForMantissaWithSign--;
    dividend = quotien;
    quotien = dividend/10;
    digit = dividend - quotien*10;
  }

  // Print sequentially mantissa and exponent
  printBase10IntegerWithDecimalMarker(buffer, availableCharsForMantissaWithSign, mantissa, 1);
  buffer[availableCharsForMantissaWithSign] = 'e';
  printBase10IntegerWithDecimalMarker(buffer+availableCharsForMantissaWithSign+1, numberOfCharExponent, exponentInBase10, -1);
  buffer[availableCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
}
