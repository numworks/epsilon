#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Matrix : public Expression {
public:
  Matrix(MatrixData * matrixData);
  Matrix(Expression ** newOperands, int numberOfOperands, int m_numberOfColumns, int m_numberOfRows, bool cloneOperands);
  ~Matrix();
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  int numberOfRows() const;
  int numberOfColumns() const;
  /* If the buffer is too small, the function fills the buffer until reaching
   * buffer size */
  int writeTextInBuffer(char * buffer, int bufferSize) override;
  Expression * createDimensionMatrix(Context& context, AngleUnit angleUnit) const;
  float trace(Context& context, AngleUnit angleUnit) const;
  float determinant(Context& context, AngleUnit angleUnit) const;
  Expression * createInverse(Context& context, AngleUnit angleUnit) const;
  Expression * createTranspose(Context& context, AngleUnit angleUnit) const;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  MatrixData * m_matrixData;
  static Complex * defaultExpression();
};

}

#endif
