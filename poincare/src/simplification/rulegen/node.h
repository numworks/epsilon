#ifndef POINCARE_SIMPLIFICATION_RULES_GENERATION_NODE_H
#define POINCARE_SIMPLIFICATION_RULES_GENERATION_NODE_H

class Rule;

#include <vector>
#include <string>

class Node {
public:
  Node(std::string * name, std::string * identifier, std::string * value, bool partialMatch = true) :
    m_name(name),
    m_identifier(identifier),
    m_value(value),
    m_partialMatch(partialMatch),
    m_children(new std::vector<Node *>()),
    m_parent(nullptr) { }
  void setChildren(std::vector<Node *> * children);
  void identifyAnonymousChildren(int * index);

  void generateSelector(Rule * rule);
  int indexOfChildrenWithIdentifier(std::string identifier);
  std::string identifier() const;
  std::string name() const { return *m_name; }
  std::string value() const;
  int intValue() const;
  int numberOfChildren() { return m_children->size(); }
  int compareTo(Node * node) const;
  void sort();
private:
  int selectorCaptureIndexInRule(Rule * rule);
  int selectorIndexInRule(Rule * rule);
  bool isCommutative();
  /*
  int generateTree(bool selector, Rule * context, int index, int indentationLevel);
  std::string generateSelectorConstructor(Rule * context);
  std::string generateBuilderConstructor(Rule * context);
  */

  std::string * m_name;
  std::string * m_identifier;
  std::string * m_value;
  bool m_partialMatch;
  std::vector<Node *> * m_children;
  Node * m_parent;
};

#endif
