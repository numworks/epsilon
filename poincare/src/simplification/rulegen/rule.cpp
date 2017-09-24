#include "rule.h"
#include <iostream>

void Rule::generate(int index) {
  std::cout << "namespace Rule" << index << " {" << std::endl;
  int selectorIndex = 0;
  m_selector->identifyAnonymousChildren(&selectorIndex);
  m_selector->generateSelector(this);
  std::cout << "constexpr Rule rule(&"
    << m_selector->identifier() << ", "
    << m_transform->name() << ", "
    << m_transform->numberOfChildren() << ");" << std::endl;
  std::cout << "}" << std::endl;
}

int Rule::indexOfIdentifierInTransform(std::string identifier) {
  return m_transform->indexOfChildrenWithIdentifier(identifier);
}
#if 0
void Rule::generate(std::string rule_name) {
  std::cout << "constexpr ExpressionSelector " << rule_name << "Selector[" << m_selector->totalDescendantCountIncludingSelf() << "] = {" << std::endl;
  m_selector->generateSelectorTree(this);
  std::cout << "};" << std::endl;

  std::cout << "constexpr ExpressionBuilder " << rule_name << "Builder[" << m_builder->totalDescendantCountIncludingSelf() << "] = {" << std::endl;
  m_builder->generateBuilderTree(this);
  std::cout << "};" << std::endl;
}
#endif
