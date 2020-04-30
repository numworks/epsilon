#ifndef ESCHER_TOOLBOX_MESSAGE_TREE_H
#define ESCHER_TOOLBOX_MESSAGE_TREE_H

#include <escher/message_tree.h>

class ToolboxMessageTree : public MessageTree {
public:
  constexpr static ToolboxMessageTree Leaf(I18n::Message label, I18n::Message text = (I18n::Message)0, bool stripInsertedText = true, I18n::Message insertedText = (I18n::Message)0) {
    return ToolboxMessageTree(
        label,
        text,
        (insertedText == (I18n::Message)0) ? label : insertedText,
        nullptr,
        0,
        stripInsertedText);
  };
  template <int N>
  constexpr static ToolboxMessageTree Node(I18n::Message label, const ToolboxMessageTree (&children)[N]) {
    return ToolboxMessageTree(
        label,
        (I18n::Message)0,
        (I18n::Message)0,
        children,
        N,
        true);
  }
  const MessageTree * childAtIndex(int index) const override { return &m_children[index]; }
  I18n::Message text() const { return m_text; }
  I18n::Message insertedText() const { return m_insertedText; }
  bool stripInsertedText() const { return m_stripInsertedText; }
private:
  constexpr ToolboxMessageTree(I18n::Message label, I18n::Message text, I18n::Message insertedText, const ToolboxMessageTree * children, int numberOfChildren, bool stripInsertedText) :
    MessageTree(label, numberOfChildren),
    m_children(children),
    m_text(text),
    m_insertedText(insertedText),
    m_stripInsertedText(stripInsertedText)
  {}
  const ToolboxMessageTree * m_children;
  I18n::Message m_text;
  I18n::Message m_insertedText;
  bool m_stripInsertedText;
};

#endif

