#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_RULE_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_RULE_H

#include "node.h"

class Rule {
public:
  Rule(Node * selector, Node * builder);
  ~Rule();
  void generate(std::string rule_name);
  Node * selector();
private:
  Node * m_selector;
  Node * m_builder;
};

#endif
