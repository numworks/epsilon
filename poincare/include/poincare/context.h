#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/float.h>

class Integer;

//TODO: We should probably make a COPY of the expressions we store

class Context {
  public:
    Context();
    virtual const Expression * expressionForSymbol(const Symbol * symbol);
    virtual const Expression * scalarExpressionForIndex(int index);
    void setExpressionForSymbolName(Expression * expression, const Symbol * symbol);
  private:
    int symbolIndex(const Symbol * symbol) const;
    static constexpr uint16_t k_maxNumberOfExpressions = 26;
    Expression * m_expressions[k_maxNumberOfExpressions];
    static Float * defaultExpression();
};

#endif
