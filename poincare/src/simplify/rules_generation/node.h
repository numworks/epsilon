#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_NODE_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_NODE_H

class Rule;

#include <vector>
#include <string>

class Node {
public:
  Node(std::string * name, std::vector<Node *> * m_children);
  ~Node();
  int generate(Rule * context = nullptr, int index = 0, int indentationLevel = 0);
  int totalDescendantCountIncludingSelf();
  int flatIndexOfChildNamed(std::string name);
protected:
  virtual void generateFields(Rule * context, std::string &indentation) = 0;
  std::string * m_name;
  std::vector<Node *> * m_children;
  Node * m_parent;
};

#endif
