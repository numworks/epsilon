#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare/matrix.h>

namespace Poincare {

class ExpressionMatrix : public Matrix {
public:
  ExpressionMatrix(MatrixData * matrixData);
  ExpressionMatrix(Expression ** newOperands, int numberOfOperands, int m_numberOfColumns, int m_numberOfRows, bool cloneOperands);
  ~ExpressionMatrix();
  ExpressionMatrix(const Matrix& other) = delete;
  ExpressionMatrix(Matrix&& other) = delete;
  ExpressionMatrix& operator=(const ExpressionMatrix& other) = delete;
  ExpressionMatrix& operator=(ExpressionMatrix&& other) = delete;
  bool hasValidNumberOfArguments() const override;
  int numberOfRows() const override;
  int numberOfColumns() const override;
  const Expression * operand(int i) const override;
  Expression * clone() const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  static Complex * defaultExpression();
  MatrixData * m_matrixData;
};

}

#endif
