#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/leaf_expression.h>
#include <math.h>

namespace Poincare {

class Complex : public LeafExpression {
public:
  static Complex Float(float x);
  static Complex Cartesian(float a, float b);
  static Complex Polar(float r, float theta);
  Complex(const char * integralPart, int integralPartLength, bool integralNegative,
        const char * fractionalPart, int fractionalPartLength,
        const char * exponent, int exponentLength, bool exponentNegative);
  Type type() const override;
  Expression * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) override;
  float a();
  float b();
  float r();
  float th();
  float absoluteValue();
  /* The parameter 'DisplayMode' refers to the way to display float 'scientific'
   * or 'auto'. The scientific mode returns float with style -1.2E2 whereas
   * the auto mode tries to return 'natural' float like (0.021) and switches
   * to scientific mode if the float is too small or too big regarding the
   * number of significant digits. If the buffer size is too small to display
   * the right number of significant digits, the function forces the scientific
   * mode and cap the number of significant digits to fit the buffer. If the
   * buffer is too small to display any float, the text representing the float
   * is truncated at the end of the buffer.
   * ConvertFloat to Text return the number of characters that have been written
   * in buffer (excluding the last \O character) */
  static int convertFloatToText(float f, char * buffer, int bufferSize, int numberOfSignificantDigits, FloatDisplayMode mode = FloatDisplayMode::Default);
  constexpr static int bufferSizeForFloatsWithPrecision(int numberOfSignificantDigits) {
    // The wors case is -1.234E-38
    return numberOfSignificantDigits + 7;
  }
private:
  Complex(float a, float b);
  constexpr static int k_numberOfSignificantDigits = 7;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  /* We here define the buffer size to write the lengthest float possible.
   * At maximum, the number has 7 significant digits so, in the worst case it
   * has the form -1.999999e-38 (7+6+1 char) (the auto mode is always
   * shorter. */
  constexpr static int k_maxFloatBufferLength = 7+6+1;
  /* We here define the buffer size to write the lengthest complex possible.
   * The worst case has the form -1.999999E-38-1.999999E-38 (13+13+1 char) */
  constexpr static int k_maxComplexBufferLength = 13+13+1;
  /* convertComplexToText and convertFloatToTextPrivate return the string length
   * of the buffer (does not count the 0 last char)*/
  int convertComplexToText(char * buffer, int bufferSize, FloatDisplayMode FloatDisplayMode) const;
  static int convertFloatToTextPrivate(float f, char * buffer, int numberOfSignificantDigits, FloatDisplayMode mode);
  /* This function prints the int i in the buffer with a '.' at the position
   * specified by the decimalMarkerPosition. It starts printing at the end of the
   * buffer and print from right to left. The integer given should be of the right
   * length to be written in bufferLength chars. If the integer is to small, the
   * empty chars on the left side are completed with '0'. If the integer is too
   * big, the printing stops when no more empty chars are available without
   * returning any warning. */
  static void printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize, int i, int decimalMarkerPosition);
  float m_a;
  float m_b;
};

}

#endif
