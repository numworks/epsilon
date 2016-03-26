#ifndef POINCARE_SIMPLIFY_SIMPLIFICATION_H
#define POINCARE_SIMPLIFY_SIMPLIFICATION_H

#include <poincare/expression.h>
#include "expression_selector.h"
#include "expression_builder.h"

class Simplification {
public:
  ExpressionSelector * m_selector;
  ExpressionBuilder * m_builder;
  Expression * simplify(Expression * expression) const;
};

#endif
