#ifndef POINCARE_SIMPLIFY_RULE_H
#define POINCARE_SIMPLIFY_RULE_H

#include "selector.h"
#include "transform.h"

namespace Poincare {

class Rule {
public:
  constexpr Rule(const Selector * s, const Transform * t) :
    m_selector(s), m_transform(t) {
    };
  bool apply(Expression * e) const;
private:
  bool immediateApply(Expression * e) const;
  const Selector * m_selector;
  const Transform * m_transform;
};

}

#endif
