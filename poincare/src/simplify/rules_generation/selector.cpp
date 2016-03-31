#include "selector.h"
#include <iostream>
#include <cassert>

Selector::Selector(Type type, std::string * name, std::vector<Selector *> * children) :
  Node(name, (std::vector<Node *> *)children),
  m_type(type)
{
}

void Selector::generateFields(Rule * context, std::string &indentation) {
  switch (m_type) {
    case Type::Variable:
      std::cout << indentation << ".m_match = ExpressionSelector::Match::Any," << std::endl;
      break;
    case Type::Wildcard:
      std::cout << indentation << ".m_match = ExpressionSelector::Match::Wildcard," << std::endl;
      break;
    case Type::ExpressionType:
      std::cout << indentation << ".m_match = ExpressionSelector::Match::TypeAndValue," << std::endl;
      std::cout << indentation << ".m_expressionType = Expression::Type::" << *m_name << "," << std::endl;
      break;
  }
}
