#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/preferences.h>
#include <poincare/integer.h>

namespace Poincare {

namespace PrintFloat {
  constexpr static int bufferSizeForFloatsWithPrecision(int numberOfSignificantDigits) {
    // The wors case is -1.234E-38
    return numberOfSignificantDigits + 7;
  }
  /* This function prints the int i in the buffer with a '.' at the position
   * specified by the decimalMarkerPosition. It starts printing at the end of the
   * buffer and print from right to left. The integer given should be of the right
   * length to be written in bufferLength chars. If the integer is to small, the
   * empty chars on the left side are completed with '0'. If the integer is too
   * big, the printing stops when no more empty chars are available without
   * returning any warning.
   * Warning: the buffer is not null terminated but is ensured to hold
   * bufferLength chars. */
  void printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize, int bufferLength, Integer i, int decimalMarkerPosition);

  /* We here define the buffer size to write the lengthest printed float
   * possible.
   * At maximum, the number has 7 significant digits so, in the worst case it
   * has the form -1.999999e-308 (7+7+1 char) (the auto mode is always
   * shorter. */
  constexpr static int k_printedFloatBufferLength = Expression::k_numberOfPrintedSignificantDigits+7+1;
  constexpr static int k_storedFloatBufferLength = Expression::k_numberOfStoredSignificantDigits+7+1;
  /* We here define the buffer size to write the lengthest printed complex
   * possible.
   * The worst case has the form -1.999999E-308*e^(-1.999999E-308*i) (14+14+7+1
   * char) */
  constexpr static int k_printedComplexBufferLength = 2*Expression::k_numberOfPrintedSignificantDigits+7+1;
}

template<typename T>
class Complex : public Evaluation<T> {
public:
  Complex() : m_a(0), m_b(0) {}
  static Complex<T> Float(T x);
  static Complex<T> Cartesian(T a, T b);
  static Complex<T> Polar(T r, T theta);
  Complex(const char * integralPart, int integralPartLength, bool integralNegative,
        const char * fractionalPart, int fractionalPartLength,
        const char * exponent, int exponentLength, bool exponentNegative);
  T toScalar() const override;
  const Complex<T> * operand(int i) const override {
    return complexOperand(i);
  }
  int numberOfRows() const override;
  int numberOfColumns() const override;
  Expression::Type type() const override;
  Complex<T> * clone() const override;
  Evaluation<T> * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = Expression::k_numberOfStoredSignificantDigits) const override;
  Evaluation<T> * createDeterminant() const override {
    return clone();
  }
  Evaluation<T> * createInverse() const override;
  Evaluation<T> * createTrace() const override {
    return clone();
  }
  T a() const;
  T b() const;
  T r() const;
  T th() const;
  Complex<T> conjugate() const;
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
  static int convertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits, Expression::FloatDisplayMode mode = Expression::FloatDisplayMode::Default);
private:
  Complex(T a, T b);
  const Complex<T> * complexOperand(int i) const override;
  ExpressionLayout * privateCreateLayout(Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat) const override;
  Evaluation<float> * privateEvaluate(Expression::SinglePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(Expression::DoublePrecision p, Context& context, Expression::AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename U> Evaluation<U> * templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const;
  /* convertComplexToText and convertFloatToTextPrivate return the string length
   * of the buffer (does not count the 0 last char)*/
  int convertComplexToText(char * buffer, int bufferSize, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, int numberOfSignificantDigits) const;
  static int convertFloatToTextPrivate(T f, char * buffer, int numberOfSignificantDigits, Expression::FloatDisplayMode mode);
  ExpressionLayout * createPolarLayout(Expression::FloatDisplayMode floatDisplayMode) const;
  ExpressionLayout * createCartesianLayout(Expression::FloatDisplayMode floatDisplayMode) const;
  T m_a;
  T m_b;
};

}

#endif
