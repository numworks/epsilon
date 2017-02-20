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
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
    float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
    int numberOfRows() const;
    int numberOfColumns() const;
    /* If the buffer is too small, the function fills the buffer until reaching
     * buffer size */
    int writeTextInBuffer(char * buffer, int bufferSize) override;
  private:
    MatrixData * m_matrixData;
    static Complex * defaultExpression();
};

}

#endif
