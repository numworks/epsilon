#ifndef APPS_TOOL_BOX_NODE_H
#define APPS_TOOL_BOX_NODE_H

#include "node.h"

class ToolBoxNode : public Node {
public:
  constexpr ToolBoxNode(const char * label = nullptr, const char * text = nullptr, const ToolBoxNode * children = nullptr, int numberOfChildren = 0) :
    Node(label, numberOfChildren),
    m_children(children),
    m_text(text)
  {
  };
  const Node * children(int index) const override;
  const char * text() const;
private:
  const ToolBoxNode * m_children;
  const char * m_text;
};

#endif

