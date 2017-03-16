#ifndef APPS_TOOLBOX_NODE_H
#define APPS_TOOLBOX_NODE_H

#include "node.h"

class ToolboxNode : public Node {
public:
  constexpr ToolboxNode(I18n::Message label = I18n::Message::Default, I18n::Message text = I18n::Message::Default, const ToolboxNode * children = nullptr, int numberOfChildren = 0) :
    Node(label, numberOfChildren),
    m_children(children),
    m_text(text)
  {
  };
  const Node * children(int index) const override;
  I18n::Message text() const;
private:
  const ToolboxNode * m_children;
  I18n::Message m_text;
};

#endif

