#ifndef POINCARE_LIST_H
#define POINCARE_LIST_H

#include <poincare/expression.h>

class List : public Expression {
  public:
    List(Expression * operand, bool cloneOperands = true);
    List(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
    ~List();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Expression * createEvaluation(Context& context) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
    void pushExpression(Expression * operand, bool cloneOperands = true);
  protected:
    int m_numberOfOperands;
    Expression ** m_operands;
};

#endif
