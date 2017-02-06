#include "simplification.h"

namespace Poincare {

Expression * Simplification::simplify(Expression * expression) const {
  ExpressionMatch matches[255]; // FIXME: The size ca be given by our compiler
  if (m_selector->match(expression, matches)) {
    return m_builder->build(matches);
  } else {
    return nullptr;
  }
}

}
