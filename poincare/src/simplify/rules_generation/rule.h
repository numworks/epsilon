#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_RULE_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_RULE_H

#include "selector.h"
#include "builder.h"

class Rule {
public:
  Rule(Selector * selector, Builder * builder);
  ~Rule();
  void generate(std::string rule_name);
  Selector * selector();
private:
  Selector * m_selector;
  Builder * m_builder;
};

#endif
