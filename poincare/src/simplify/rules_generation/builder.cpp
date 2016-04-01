#include "builder.h"
#include "rule.h"
#include <iostream>
#include <cassert>

Builder::Builder(Type type, std::string * name, std::vector<Builder *> * children) :
  Node(name, (std::vector<Node *> *)children),
  m_type(type)
{
}

void Builder::generateFields(Rule * context, std::string &indentation) {
  Selector * selector = context->selector();
  switch (m_type) {
    case Type::ExpressionType:
      std::cout << indentation << ".m_action = ExpressionBuilder::Action::BuildFromTypeAndValue," << std::endl;
      std::cout << indentation << ".m_expressionType = Expression::Type::" << *m_name << "," << std::endl;
      break;
    case Type::Variable:
      std::cout << indentation << ".m_action = ExpressionBuilder::Action::Clone," << std::endl;
      assert(m_children->size() == 0);
      std::cout << indentation << ".m_matchIndex = " << selector->flatIndexOfChildNamed(*m_name) << "," << std::endl;
      break;
    case Type::Wildcard:
      std::cout << indentation << ".m_action = ExpressionBuilder::Action::BringUpWildcard," << std::endl;
      assert(m_children->size() == 0);
      std::cout << indentation << ".m_matchIndex = " << selector->flatIndexOfChildNamed(*m_name) << "," << std::endl;
      break;
  }
}
