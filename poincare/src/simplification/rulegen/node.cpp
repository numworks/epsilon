#include "node.h"
#include "rule.h"
#include <iostream>
#include <sstream>
#include <cassert>

void Node::setChildren(std::vector<Node *> * children) {
  delete m_children;
  m_children = children;
  for (Node * child : *m_children) {
    child->m_parent = this;
  }
}

void Node::identifyAnonymousChildren(int * index) {
  if (m_identifier == nullptr) {
    m_identifier = new std::string("s" + std::to_string(*index));
    (*index)++;
  }
  for (Node * child : *m_children) {
    child->identifyAnonymousChildren(index);
  }
}

// Generation

void Node::generateSelector(Rule * rule) {
  int i = 0;

  for (Node * child : *m_children) {
    child->generateSelector(rule);
  }

  if (m_children->size() > 0) {
    std::cout
      << "constexpr const Selector * "
      << identifier() << "Children[] = {";
    for (Node * child : *m_children) {
      std::cout  << "&" << child->identifier();
      if (child != m_children->back()) {
        std::cout << ", ";
      }
    }
    std::cout << "};" << std::endl;
  }
  std::cout << "constexpr TypeSelector " << identifier() << "(Expression::Type::" << *m_name << ", " << rule->indexOfIdentifierInTransform(*m_identifier);
  if (m_children->size() > 0) {
    std::cout << ", " << identifier() <<"Children, " << m_children->size();
  }
  std::cout << ");" << std::endl;
}

void Node::generateTransform() {
  std::cout << "constexpr " << *m_name  << " transform;" << std::endl;
}

int Node::indexOfChildrenWithIdentifier(std::string identifier) {
  for (int i=0; i<m_children->size(); i++) {
    if (*(m_children->at(i)->m_identifier) == identifier) {
      return i;
    }
  }
  return -1;
}


std::string Node::identifier() {
  if (m_identifier) {
    return *m_identifier;
  }
  return "NOIDEA";
}



#if 0

std::string Node::generateSelectorConstructor(Rule * context) {
  std::ostringstream result;
  switch (m_type) {
    case Node::Type::Any:
      switch (m_referenceMode) {
        case Node::ReferenceMode::None:
        case Node::ReferenceMode::SingleNode:
          {
            // We try to see if we already saw this node before.
            Node * selector = context->selector();
            int index = selector->flatIndexOfChildNamed(*m_referenceName);
            int my_index = selector->flatIndexOfChildRef(this);
            if (index >= 0 && index < my_index) {
              result << "ExpressionSelector::SameAs(" << index << ", ";
            } else {
              result << "ExpressionSelector::Any(";
            }
          }
          break;
        case Node::ReferenceMode::Wildcard:
          result << "ExpressionSelector::Wildcard(";
          break;
      }
      break;
    case Node::Type::Expression:
      if (m_value == nullptr) {
        result << "ExpressionSelector::Type(Expression::Type::" << *m_typeName << ", ";
      } else {
        result << "ExpressionSelector::TypeAndValue(Expression::Type::" << *m_typeName << ", " << *m_value << ", ";
      }
      break;
    case Node::Type::Generator:
      //assert(false);
      break;
  }
  result << m_children->size() << ")";
  return result.str();
}

std::string Node::generateBuilderConstructor(Rule * context) {
  Node * selector = context->selector();
  std::ostringstream result;
  switch (m_type) {
    case Node::Type::Any:
      switch (m_referenceMode) {
        case Node::ReferenceMode::None:
          assert(false);
          break;
        case Node::ReferenceMode::SingleNode:
          assert(m_referenceName != nullptr);
          result << "ExpressionBuilder::Clone(" << selector->flatIndexOfChildNamed(*m_referenceName) << ", " ;
          break;
        case Node::ReferenceMode::Wildcard:
          result << "ExpressionBuilder::BringUpWildcard(" << selector->flatIndexOfChildNamed(*m_referenceName) << ", ";
          break;
      }
      break;
    case Node::Type::Expression:
      if (m_value == nullptr) {
        // Here we could assert that m_typeName is among the kinds that expect no value
        result << "ExpressionBuilder::BuildFromType(Expression::Type::" << *m_typeName << ", ";
      } else {
        // Here we could assert that m_typeName is among the kinds that expect a value
        result << "ExpressionBuilder::BuildFromTypeAndValue(Expression::Type::" << *m_typeName << ", " << *m_value << ", ";
      }
      break;
    case Node::Type::Generator:
      result << "ExpressionBuilder::CallExternalGenerator(SimplificationGenerator::" << *m_typeName << ", ";
      //assert(false);
      break;
  }
  result << m_children->size() << ")";
  return result.str();
}

int Node::flatIndexOfChildRef(Node * node) {
  if (m_referenceName != nullptr && node == this) {
    return 0;
  }
  int sum=1;
  for (Node * child : *m_children) {
    int index = child->flatIndexOfChildRef(node);
    if (index >= 0) {
      return sum+index;
    } else {
      sum += child->totalDescendantCountIncludingSelf();
    }
  }
  return -1;
}

int Node::flatIndexOfChildNamed(std::string name) {
  if (m_referenceName != nullptr && *m_referenceName == name) {
    return 0;
  }
  int sum=1;
  for (Node * child : *m_children) {
    int index = child->flatIndexOfChildNamed(name);
    if (index >= 0) {
      return sum+index;
    } else {
      sum += child->totalDescendantCountIncludingSelf();
    }
  }
  return -1;
}

int Node::totalDescendantCountIncludingSelf() {
  int result = 1;
  for (Node * child : *m_children) {
    result += child->totalDescendantCountIncludingSelf();
  }
  return result;
}

std::string indentation_string(int i) {
    int indentBy = 2;
    std::string result;
    result.reserve(i*indentBy);
    for (int j=0; j<i; j++) {
      for (int k=0; k<indentBy; k++) {
        result += " ";
      }
    }
    return result;
}

void Node::generateSelectorTree(Rule * context) {
  generateTree(true, context, 0, 0);
}

void Node::generateBuilderTree(Rule * context) {
  generateTree(false, context, 0, 0);
}

int Node::generateTree(bool selector, Rule * context, int index, int indentationLevel) {
  std::string indentation = indentation_string(indentationLevel);
  std::cout << indentation;
  if (selector) {
    std::cout << generateSelectorConstructor(context);
  } else {
    std::cout << generateBuilderConstructor(context);
  }
  std::cout << ", // #" << index << std::endl;
  int generatedCount = 1;
  for (Node * child : *m_children) {
    generatedCount += child->generateTree(selector, context, index+generatedCount, indentationLevel+1);
  }
  return generatedCount;
}
#endif
