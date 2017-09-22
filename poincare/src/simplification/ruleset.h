#ifndef POINCARE_SIMPLIFICATION_RULESET_H
#define POINCARE_SIMPLIFICATION_RULESET_H

#include "rule.h"
#include "selector/type_selector.h"
#include "transform/integer_addition_transform.h"
#include "transform/merge_dynamic_hierarchy_transform.h"

namespace Poincare {
namespace Simplification {

class Ruleset {
public:
  constexpr Ruleset(const Rule rules[], int numberOfRules) :
    m_rules(rules),
    m_numberOfRules(numberOfRules) {};
  const Rule * ruleAtIndex(int i) const { return m_rules+i; };
  int numberOfRules() const { return m_numberOfRules; };
private:
  const Rule * m_rules;
  int m_numberOfRules;
};

}
}

#endif
