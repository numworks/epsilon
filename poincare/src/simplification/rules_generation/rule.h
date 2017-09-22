#ifndef POINCARE_SIMPLIFICATION_RULES_GENERATION_RULE_H
#define POINCARE_SIMPLIFICATION_RULES_GENERATION_RULE_H

#include "node.h"

class Rule {
public:
  Rule(Node * selector, Node * transform) :
    m_selector(selector), m_transform(transform) {}
  void generate(int index);
  void identifyAnonymousSelectors();
  int indexOfIdentifierInTransform(std::string identifier);
private:
  Node * m_selector;
  Node * m_transform;
};

#endif
