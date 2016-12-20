#ifndef POINCARE_FLOAT_H
#define POINCARE_FLOAT_H

#include <poincare/leaf_expression.h>

class Float : public LeafExpression {
public:
  Float(float f);
  Float(const char * integralPart, bool integralNegative,
        const char * fractionalPart,
        const char * exponent, bool exponentNegative);

  ExpressionLayout * createLayout() const override;
  float approximate(Context& context) const override;
  Expression * evaluate(Context& context) const override;
  Type type() const override;
  Expression * clone() const override;
  bool valueEquals(const Expression * e) const override;

  enum class DisplayMode {
    Scientific,
    Decimal
  };
  void setNumberOfDigitsInMantissa(int numberOfDigits);
  /* The parameter 'DisplayMode' refers to the way to display float 'scientific' or
   * 'decimal'. The code only handles 'scientific' so far. If the buffer size is
   * small, the function does nothing. */
  int convertFloatToText(char * buffer, int bufferSize, int numberOfDigitsInMantissa, DisplayMode mode = DisplayMode::Scientific) const;
  int writeTextInBuffer(char * buffer, int bufferSize) override;
private:
  constexpr static int k_maxBufferLength = 14;
  /* This function prints the int i in the buffer with a '.' at the position
   * specified by the decimalMarkerPosition. It starts printing at the end of the
   * buffer and print from right to left. The integer given should be of the right
   * length to be written in bufferLength chars. If the integer is to small, the
   * empty chars on the left side are completed with '0'. If the integer is too
   * big, the printing stops when no more empty chars are available without
   * returning any warning. */
  static void printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize, int i, int decimalMarkerPosition);

  float m_float;
  int m_numberOfDigitsInMantissa;

};

#endif
