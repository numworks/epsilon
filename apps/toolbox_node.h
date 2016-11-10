#ifndef APPS_TOOLBOX_NODE_H
#define APPS_TOOLBOX_NODE_H

#include "node.h"

class ToolboxNode : public Node {
public:
  constexpr ToolboxNode(const char * label = nullptr, const char * text = nullptr, const ToolboxNode * children = nullptr, int numberOfChildren = 0) :
    Node(label, numberOfChildren),
    m_children(children),
    m_text(text)
  {
  };
  const Node * children(int index) const override;
  const char * text() const;
private:
  const ToolboxNode * m_children;
  const char * m_text;
};

#endif

