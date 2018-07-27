#ifndef POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H
#define POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class VerticalOffsetLayoutNode : public LayoutNode {
public:
  enum class Type {
    Subscript,
    Superscript
  };

  VerticalOffsetLayoutNode(Type type = Type::Superscript) :
    LayoutNode(),
    m_type(type)
  {}

  // VerticalOffsetLayoutNode
  Type type() const { return m_type; }
  void setType(Type type) { m_type = type; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool mustHaveLeftSibling() const override { return true; }
  bool isVerticalOffset() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(VerticalOffsetLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if TREE_LOG
  const char * description() const override {
    return m_type == Type::Subscript ? "Subscript" : "Superscript";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_indiceHeight = 5;
  constexpr static KDCoordinate k_separationMargin = 5;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  LayoutNode * indiceLayout() {
    assert(numberOfChildren() == 1);
    return childAtIndex(0);
  }
  LayoutNode * baseLayout();
  Type m_type;
};

class VerticalOffsetLayoutRef : public LayoutReference {
public:
  VerticalOffsetLayoutRef(TreeNode * t) : LayoutReference(t) {}
  VerticalOffsetLayoutRef(LayoutRef l, VerticalOffsetLayoutNode::Type type) :
    VerticalOffsetLayoutRef()
  {
    if (!(node()->isAllocationFailure())) {
      static_cast<VerticalOffsetLayoutNode *>(node())->setType(type);
    }
    addChildTreeAtIndex(l, 0);
  }
private:
  VerticalOffsetLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<VerticalOffsetLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
