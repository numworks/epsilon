#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/matrix_data.h>

namespace Poincare {

class Matrix : public Expression {
public:
  int numberOfOperands() const override;
  virtual int numberOfRows() const = 0;
  virtual int numberOfColumns() const = 0;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif
