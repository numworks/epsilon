#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_NODE_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_NODE_H

class Rule;

#include <vector>
#include <string>

class Node {
public:
  enum class Type {
    Expression,
    Generator,
    Any
  };
  enum class ReferenceMode {
    None,
    SingleNode,
    Wildcard
  };

  // Creating Nodes
  Node(Type type, std::string * typeName = nullptr);
  ~Node();
  void setReference(ReferenceMode mode, std::string * referenceName);
  void setValue(std::string * value);
  void setChildren(std::vector<Node *> * children);

  int totalDescendantCountIncludingSelf();
  int flatIndexOfChildNamed(std::string name);
  int flatIndexOfChildRef(Node * node);

  void generateSelectorTree(Rule * context);
  void generateBuilderTree(Rule * context);
private:
  int generateTree(bool selector, Rule * context, int index, int indentationLevel);
  std::string generateSelectorConstructor(Rule * context);
  std::string generateBuilderConstructor(Rule * context);

  Type m_type;
  std::string * m_typeName;
  ReferenceMode m_referenceMode;
  std::string * m_referenceName;
  std::string * m_value;
  std::vector<Node *> * m_children;
  Node * m_parent;
};

#endif
