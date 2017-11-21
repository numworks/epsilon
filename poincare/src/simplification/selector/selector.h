#ifndef POINCARE_SIMPLIFICATION_SELECTOR_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SELECTOR_SELECTOR_H

#include <poincare/expression.h>

namespace Poincare {
namespace Simplification {

class Combination;

class Selector {
public:
  constexpr Selector(int captureIndex = -1, const Selector * const * children = nullptr, int numberOfChildren = 0, bool childrenPartialMatch = true) :
    m_captureIndex(captureIndex), m_children(children), m_numberOfChildren(numberOfChildren), m_childrenPartialMatch(childrenPartialMatch) {}
  virtual bool acceptsLocationInCombination(const Combination * combination, int location) const;
  virtual bool immediateMatch(const Expression * e) const = 0;
  bool match(const Expression * e, Expression ** captures, int captureLength) const;
private:
  int m_captureIndex;
  const Selector * const * m_children;
  int m_numberOfChildren;
  bool m_childrenPartialMatch;
};

}
}

#endif
