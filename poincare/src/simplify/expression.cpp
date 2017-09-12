#include <poincare/expression.h>
#include "ruleset.h"
#include "../expression_debug.h"
#include <iostream>

namespace Poincare {

void Expression::simplify() {
  sort();
  // Only unconditional simplify
  int ruleIndex = 0;
  while (ruleIndex < DemoRuleSet.numberOfRules()) {
    const Rule * rule = DemoRuleSet.ruleAtIndex(ruleIndex++);
    if (rule->apply(this)) {
      this->sort();
      std::cout << "-----" << std::endl;
      print_expression(this, 0);
      std::cout << "-----" << std::endl;
      ruleIndex = 0;
    }
  }
}

}
