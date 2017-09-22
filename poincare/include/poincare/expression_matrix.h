#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare/matrix.h>
#include <assert.h>

namespace Poincare {

class ExpressionMatrix : public Matrix {
public:
  ExpressionMatrix(MatrixData * matrixData);
  ExpressionMatrix(Expression ** newOperands, int numberOfOperands, int m_numberOfRows, int m_numberOfColumns, bool cloneOperands);
  ~ExpressionMatrix();
  ExpressionMatrix(const Matrix& other) = delete;
  ExpressionMatrix(Matrix&& other) = delete;
  ExpressionMatrix& operator=(const ExpressionMatrix& other) = delete;
  ExpressionMatrix& operator=(ExpressionMatrix&& other) = delete;

  /* Expression */
  Type type() const override;
  Expression * clone() const override;
  bool isCommutative() const override;

  // TODO: Remove these 2 functions
  void replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand) override {
    assert(false);
  }
  void swapOperands(int i, int j) override { assert(false); }

  /* Evaluation */
  int numberOfRows() const override;
  int numberOfColumns() const override;
  const Expression * operand(int i) const override;
private:
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  MatrixData * m_matrixData;
};

}

#endif
