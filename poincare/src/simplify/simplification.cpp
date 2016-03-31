#include "simplification.h"

Expression * Simplification::simplify(Expression * expression) const {
  ExpressionMatch * matches[255]; // FIXME: The size ca be given by our compiler
  if (m_selector->match(expression, matches)) {
    if (expression->numberOfOperands() == m_selector->m_numberOfChildren) {
      return m_builder->build(matches);
    }
  } else {
    return nullptr;
  }
}
