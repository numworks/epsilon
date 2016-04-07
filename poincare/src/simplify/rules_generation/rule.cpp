#include "rule.h"
#include <iostream>

Rule::Rule(Node * selector, Node * builder) :
  m_selector(selector), m_builder(builder) {
}

Rule::~Rule() {
  delete m_builder;
  delete m_selector;
}

Node * Rule::selector() {
  return m_selector;
}

void Rule::generate(std::string rule_name) {
  std::cout << "constexpr ExpressionSelector " << rule_name << "Selector[" << m_selector->totalDescendantCountIncludingSelf() << "] = {" << std::endl;
  m_selector->generateSelectorTree(this);
  std::cout << "};" << std::endl;

  std::cout << "constexpr ExpressionBuilder " << rule_name << "Builder[" << m_builder->totalDescendantCountIncludingSelf() << "] = {" << std::endl;
  m_builder->generateBuilderTree(this);
  std::cout << "};" << std::endl;
}
