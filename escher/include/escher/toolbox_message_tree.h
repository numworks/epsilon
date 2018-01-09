#ifndef ESCHER_TOOLBOX_MESSAGE_TREE_H
#define ESCHER_TOOLBOX_MESSAGE_TREE_H

#include <escher/message_tree.h>
#include <poincare_layouts.h>

class ToolboxMessageTree : public MessageTree {
public:
  constexpr ToolboxMessageTree(I18n::Message label = (I18n::Message)0, I18n::Message text = (I18n::Message)0, I18n::Message insertedText = (I18n::Message)0, const ToolboxMessageTree * children = nullptr, int numberOfChildren = 0, int * pointedLayoutPath = nullptr, int pointedLayoutPathLength = 0) :
    MessageTree(label, numberOfChildren),
    m_children(children),
    m_text(text),
    m_insertedText(insertedText),
    m_pointedLayoutPath(pointedLayoutPath),
    m_pointedLayoutPathLength(pointedLayoutPathLength)
  {
  };
  const MessageTree * children(int index) const override { return &m_children[index]; }
  I18n::Message text() const { return m_text; }
  I18n::Message insertedText() const { return m_insertedText; }
  int * pointedPath() const { return m_pointedLayoutPath; }
  int pointedPathLength() const { return m_pointedLayoutPathLength; }
private:
  const ToolboxMessageTree * m_children;
  I18n::Message m_text;
  I18n::Message m_insertedText;
  int * m_pointedLayoutPath;
  int m_pointedLayoutPathLength;
};

#endif

