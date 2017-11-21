#include "rule.h"

namespace Poincare {
namespace Simplification {

bool Rule::apply(Expression * e) const {
  if (immediateApply(e)) {
    return true;
  }

  for (int i=0; i<e->numberOfOperands(); i++) {
    Expression * child = (Expression *)(e->operand(i));
    // FIXME: Remove const-ness from operand()
    if (apply(child)) {
      return true;
    }
  }

  return false;
}

bool Rule::immediateApply(Expression * e) const {
  Expression * captures[m_captureLength];
  if (m_selector->match(e, captures, m_captureLength)) {
    return m_transform(captures);
  }
  return false;
}

}
}
