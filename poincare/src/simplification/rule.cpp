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
  Expression * m[5]; // En fait, 5 est un upper-bound très facilement calculable par notre compilateur de regle. C'est le max du nombre de capture de toutes les règles. Oui, on pourrait faire un truc dynamique qui n'alloue que ce dont le selecteur actuel a besoin, mais bon...
  if (m_selector->match(e, m)) {
    m_transform(m);
    return true;
  }
  return false;
}

}
}
