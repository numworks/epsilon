#ifndef ESCHER_TOOLBOX_MESSAGE_TREE_H
#define ESCHER_TOOLBOX_MESSAGE_TREE_H

#include <escher/message_tree.h>
#include <poincare/layout.h>

namespace Escher {

/* TODO: Currently Escher needs to know the closed list of the
 * ToolboxMessageTree children to build the union. As an alternative we could
 * template over the union to allow the user to provide the list. */
union ToolboxMessage;

class ToolboxMessageTree : public MessageTree {
 public:
  using MessageTree::MessageTree;
  const MessageTree* childAtIndex(int index) const override { return nullptr; }
  virtual const ToolboxMessage* childrenList() const { return nullptr; }
  virtual Poincare::Layout layout() const { return {}; }
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

  constexpr I18n::Message text() const override { return m_text; }
  I18n::Message insertedText() const override { return m_insertedText; }
  bool stripInsertedText() const override { return m_stripInsertedText; }

 private:
  I18n::Message m_text;
  I18n::Message m_insertedText;
  bool m_stripInsertedText;
};

class ToolboxMessageMath : public ToolboxMessageTree {
 public:
  constexpr ToolboxMessageMath(const Poincare::Internal::Tree* layout,
                               I18n::Message text)
      : ToolboxMessageTree(text), m_layout(layout){};

 private:
  // text is stored in label since its layout hides the label
  constexpr I18n::Message text() const override { return label(); }
  Poincare::Layout layout() const override { return m_layout; }

 private:
  const Poincare::Internal::Tree* m_layout;
};

class ToolboxMessageNodeDirect : public ToolboxMessageTree {
 public:
  constexpr ToolboxMessageNodeDirect(I18n::Message label,
                                     const ToolboxMessage* const children,
                                     int numberOfChildren)
      : ToolboxMessageTree(label),
        m_directChildren(children),
        m_numberOfChildren(numberOfChildren) {}

  int numberOfChildren() const override { return m_numberOfChildren; }
  const MessageTree* childAtIndex(int index) const override;
  const ToolboxMessage* childrenList() const override {
    return m_directChildren;
  }

 private:
  const ToolboxMessage* const m_directChildren;
  const int16_t m_numberOfChildren;
};

class ToolboxMessageNodeIndirect : public ToolboxMessageTree {
 public:
  constexpr ToolboxMessageNodeIndirect(I18n::Message label,
                                       const ToolboxMessage* const* children,
                                       int numberOfChildren)
      : ToolboxMessageTree(label),
        m_indirectChildren(children),
        m_numberOfChildren(numberOfChildren) {}

  int numberOfChildren() const override { return m_numberOfChildren; }
  const MessageTree* childAtIndex(int index) const override {
    return reinterpret_cast<const MessageTree*>(m_indirectChildren[index]);
  }

 private:
  const ToolboxMessage* const* m_indirectChildren;
  const int16_t m_numberOfChildren;
};

template <int N>
constexpr ToolboxMessageNodeDirect ToolboxMessageNode(
    I18n::Message label, const ToolboxMessage (&children)[N],
    bool fork = false) {
  return ToolboxMessageNodeDirect(label, children, fork ? -N : N);
}

template <int N>
constexpr ToolboxMessageNodeIndirect ToolboxMessageNode(
    I18n::Message label, const ToolboxMessage* const (&children)[N],
    bool fork = false) {
  return ToolboxMessageNodeIndirect(label, children, fork ? -N : N);
}

/* For size efficiency ToolboxMessageTrees are constructed in constexpr in
 * arrays. We need elements of the array to have the same size and some menus
 * mix leaves and nodes. The union ToolboxMessage gathers the subclasses of
 * ToolboxMessageTree and is the type of the array elements. */
union ToolboxMessage {
  constexpr ToolboxMessage(ToolboxMessageLeaf leaf) : leaf(leaf) {}
  constexpr ToolboxMessage(ToolboxMessageMath math) : math(math) {}
  constexpr ToolboxMessage(ToolboxMessageNodeDirect node) : nodeDirect(node) {}
  constexpr ToolboxMessage(ToolboxMessageNodeIndirect node)
      : nodeIndirect(node) {}
  const ToolboxMessageTree* toMessageTree() const {
    return reinterpret_cast<const ToolboxMessageTree*>(this);
  }
  ToolboxMessageLeaf leaf;
  ToolboxMessageMath math;
  ToolboxMessageNodeDirect nodeDirect;
  ToolboxMessageNodeIndirect nodeIndirect;
};

// This method needs to be after the union's definition to know the union's size
inline const MessageTree* ToolboxMessageNodeDirect::childAtIndex(
    int index) const {
  return reinterpret_cast<const MessageTree*>(m_directChildren + index);
}

}  // namespace Escher
#endif
