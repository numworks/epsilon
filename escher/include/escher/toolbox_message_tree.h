#ifndef ESCHER_TOOLBOX_MESSAGE_TREE_H
#define ESCHER_TOOLBOX_MESSAGE_TREE_H

#include <escher/message_tree.h>

class ToolboxMessageTree : public MessageTree {
public:
  constexpr ToolboxMessageTree(I18n::Message label = (I18n::Message)0, I18n::Message text = (I18n::Message)0, I18n::Message insertedText = (I18n::Message)0, const ToolboxMessageTree * children = nullptr, int numberOfChildren = 0) :
    MessageTree(label, numberOfChildren),
    m_children(children),
    m_text(text),
    m_insertedText(insertedText)
  {
  };
  const MessageTree * children(int index) const override;
  I18n::Message text() const;
  I18n::Message insertedText() const;
private:
  const ToolboxMessageTree * m_children;
  I18n::Message m_text;
  I18n::Message m_insertedText;
};

#endif

