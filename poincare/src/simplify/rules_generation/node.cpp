#include "node.h"
#include "rule.h"
#include <iostream>
#include <sstream>
#include <cassert>

Node::Node(Type type, std::string * typeName) :
  m_type(type),
  m_typeName(typeName),
  m_referenceMode(ReferenceMode::None),
  m_referenceName(nullptr),
  m_value(nullptr),
  m_parent(nullptr)
{
  m_children = new std::vector<Node *>();
}

Node::~Node() {
  delete m_children;
}

void Node::setReference(ReferenceMode mode, std::string * referenceName) {
  assert(m_referenceName == nullptr);
  m_referenceName = referenceName;
  m_referenceMode = mode;
}

void Node::setValue(std::string * value) {
  assert(m_value == nullptr);
  m_value = value;
}

void Node::setChildren(std::vector<Node *> * children) {
  assert(m_children->size() == 0);
  delete m_children;
  m_children = children;
  for (Node * child : *m_children) {
    child->m_parent = this;
  }
}

// Generation

std::string Node::generateSelectorConstructor(Rule * context) {
  std::ostringstream result;
  switch (m_type) {
    case Node::Type::Any:
      switch (m_referenceMode) {
        case Node::ReferenceMode::None:
        case Node::ReferenceMode::SingleNode:
          result << "ExpressionSelector::Any(";
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
        result << "ExpressionSelector::TypeAndValue(Expression::Type::" << *m_typeName << ", " << *m_value << ",";
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
