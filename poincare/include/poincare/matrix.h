#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/expression.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Matrix : public Expression {
public:
  virtual const Expression * operand(int i) const override = 0;
  int numberOfOperands() const override;
  virtual int numberOfRows() const = 0;
  virtual int numberOfColumns() const = 0;
  /* If the buffer is too small, the function fills the buffer until reaching
   * buffer size */
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = k_numberOfStoredSignificantDigits) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif
