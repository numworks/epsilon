#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/list.h>

class Matrix : public Expression {
  public:
    Matrix(List * list, bool cloneOperands = true);
    Matrix(Expression ** newOperands, int numberOfOperands, int m_numberOfColumns, int m_numberOfRows, bool cloneOperands);
    ~Matrix();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Expression * createEvaluation(Context& context) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
    void pushList(List * list, bool cloneOperands = true);
    int numberOfRows();
    int numberOfColumns();
  protected:
    void setText() override;
  private:
    int m_numberOfRows;
    int m_numberOfColumns;
    Expression ** m_operands;
    static Integer * defaultExpression();
};

#endif
