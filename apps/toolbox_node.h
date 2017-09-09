#ifndef APPS_TOOLBOX_NODE_H
#define APPS_TOOLBOX_NODE_H

#include "node.h"

class ToolboxNode : public Node {
public:
  constexpr ToolboxNode(const I18n::Message *label = &I18n::Common::Default, const I18n::Message *text = &I18n::Common::Default, const I18n::Message *insertedText = &I18n::Common::Default, const ToolboxNode * children = nullptr, int numberOfChildren = 0) :
    Node(label, numberOfChildren),
    m_children(children),
    m_text(text),
    m_insertedText(insertedText)
  {
  };
  const Node * children(int index) const override;
  const I18n::Message *text() const;
  const I18n::Message *insertedText() const;
private:
  const ToolboxNode * m_children;
  const I18n::Message *m_text;
  const I18n::Message *m_insertedText;
};

#endif

