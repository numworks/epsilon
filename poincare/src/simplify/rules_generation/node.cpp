#include "builder.h"
#include <iostream>
#include <cassert>

Node::Node(std::string * name, std::vector<Node *> * children) :
  m_name(name),
  m_children(children)
{
  if (children == nullptr) {
    m_children = new std::vector<Node *>();
  }
}

Node::~Node() {
  delete m_children;
}

int Node::flatIndexOfChildNamed(std::string name) {
  if (name == *m_name) {
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

int Node::generate(Rule * context, int index, int indentationLevel) {
  std::string indentation = indentation_string(indentationLevel);
  std::string nextIndentation = indentation_string(indentationLevel+1);
  std::cout << indentation << "{" << std::endl;
  std::cout << nextIndentation << "// #" << index << std::endl;
  this->generateFields(context, nextIndentation);
  std::cout << nextIndentation << ".m_numberOfChildren = " << m_children->size() << "," << std::endl;
  std::cout << indentation << "}," << std::endl;
  int generatedCount = 1;
  for (Node * child : *m_children) {
    generatedCount += child->generate(context, index+generatedCount, indentationLevel+1);
  }
  return generatedCount;
}
