#ifndef ESCHER_TOOLBOX_MESSAGE_TREE_H
#define ESCHER_TOOLBOX_MESSAGE_TREE_H

#include <escher/message_tree.h>

namespace Escher {

union ToolboxMessage;

class ToolboxMessageTree : public MessageTree {
 public:
  using MessageTree::MessageTree;
  const MessageTree *childAtIndex(int index) const override { return nullptr; }
  virtual const ToolboxMessage *childrenList() const { return nullptr; }
  virtual constexpr I18n::Message text() const { return I18n::Message(0); }
  virtual I18n::Message insertedText() const { return I18n::Message(0); }
  virtual bool stripInsertedText() const { return true; }
  bool isFork() const { return numberOfChildren() < 0; }
};

class ToolboxMessageLeaf : public ToolboxMessageTree {
 public:
  constexpr ToolboxMessageLeaf(I18n::Message label,
                               I18n::Message text = (I18n::Message)0,
                               bool stripInsertedText = true,
                               I18n::Message insertedText = (I18n::Message)0)
      : ToolboxMessageTree(label),
        m_text(text),
        m_insertedText(insertedText == (I18n::Message)0 ? label : insertedText),
        m_stripInsertedText(stripInsertedText){};

 private:
  constexpr I18n::Message text() const override { return m_text; }
  I18n::Message insertedText() const override { return m_insertedText; }
  bool stripInsertedText() const override { return m_stripInsertedText; }

 private:
  I18n::Message m_text;
  I18n::Message m_insertedText;
  bool m_stripInsertedText;
};

class ToolboxMessageNode : public ToolboxMessageTree {
 public:
  template <int N>
  constexpr ToolboxMessageNode(I18n::Message label,
                               const ToolboxMessage (&children)[N],
                               bool fork = false)
      : ToolboxMessageNode(label, children, fork ? -N : N) {}
  template <int N>
  constexpr ToolboxMessageNode(I18n::Message label,
                               const ToolboxMessage *const (&children)[N],
                               bool fork = false)
      : ToolboxMessageNode(label, children, fork ? -N : N) {}

 private:
  constexpr ToolboxMessageNode(I18n::Message label,
                               const ToolboxMessage *const children,
                               int numberOfChildren)
      : ToolboxMessageTree(label, numberOfChildren),
        m_children(children),
        m_childrenConsecutive(true) {}
  constexpr ToolboxMessageNode(I18n::Message label,
                               const ToolboxMessage *const *children,
                               int numberOfChildren)
      : ToolboxMessageTree(label, numberOfChildren),
        m_children(children),
        m_childrenConsecutive(false) {}

  const MessageTree *childAtIndex(int index) const override;
  const ToolboxMessage *childrenList() const override {
    return m_childrenConsecutive ? m_children.m_direct : nullptr;
  }

  union Children {
   public:
    constexpr Children(const ToolboxMessage *const children)
        : m_direct(children) {}
    constexpr Children(const ToolboxMessage *const *children)
        : m_indirect(children) {}
    const ToolboxMessage *const m_direct;
    const ToolboxMessage *const *m_indirect;
  };

 private:
  const Children m_children;
  const bool m_childrenConsecutive;
};

union ToolboxMessage {
  constexpr ToolboxMessage(ToolboxMessageLeaf leaf) : leaf(leaf) {}
  constexpr ToolboxMessage(ToolboxMessageNode node) : node(node) {}
  const ToolboxMessageTree *toMessageTree() const {
    return reinterpret_cast<const ToolboxMessageTree *>(this);
  }
  ToolboxMessageLeaf leaf;
  ToolboxMessageNode node;
};

inline const MessageTree *ToolboxMessageNode::childAtIndex(int index) const {
  return reinterpret_cast<const MessageTree *>(
      m_childrenConsecutive ? m_children.m_direct + index
                            : m_children.m_indirect[index]);
}

}  // namespace Escher
#endif
