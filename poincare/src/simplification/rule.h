#ifndef POINCARE_SIMPLIFICATION_RULE_H
#define POINCARE_SIMPLIFICATION_RULE_H

#include "selector/selector.h"
#include "transform/transform.h"

namespace Poincare {
namespace Simplification {

class Rule {
public:
  constexpr Rule(const Selector * s, Transform t, int captureLength) :
    m_selector(s), m_transform(t), m_captureLength(captureLength) {
    };
  bool apply(Expression * e) const;
private:
  bool immediateApply(Expression * e) const;
  const Selector * m_selector;
  Transform m_transform;
  int m_captureLength;
};

}
}

#endif
