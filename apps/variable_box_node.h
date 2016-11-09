#ifndef APPS_VARIABLE_BOX_NODE_H
#define APPS_VARIABLE_BOX_NODE_H

#include "node.h"
#include <poincare.h>

class VariableBoxNode : public Node {
public:
  constexpr VariableBoxNode(const char * label = nullptr, const VariableBoxNode * children = nullptr, int numberOfChildren = 0) :
    Node(label, numberOfChildren),
    m_children(children)
  {
  };
  const Node * children(int index) const override;
private:
  const VariableBoxNode * m_children;
};

#endif

