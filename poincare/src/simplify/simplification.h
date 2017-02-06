#ifndef POINCARE_SIMPLIFY_SIMPLIFICATION_H
#define POINCARE_SIMPLIFY_SIMPLIFICATION_H

#include <poincare/expression.h>
#include "expression_selector.h"
#include "expression_builder.h"

namespace Poincare {

class Simplification {
public:
  constexpr Simplification(ExpressionSelector * m_selector, ExpressionBuilder * m_builder);
  Expression * simplify(Expression * expression) const;
private:
  ExpressionSelector * m_selector;
  ExpressionBuilder * m_builder;
};

constexpr Simplification::Simplification(
  ExpressionSelector * selector,
  ExpressionBuilder * builder)
  :
  m_selector(selector),
  m_builder(builder) {
}

}

#endif
