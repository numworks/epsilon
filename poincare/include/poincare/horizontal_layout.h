#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

/* WARNING: A HorizontalLayout should never have a HorizontalLayout child. For
 * instance, use addOrMergeChildAtIndex to add a LayoutNode safely. */

class HorizontalLayoutNode final : public LayoutNode {
  friend class Layout;
public:

  HorizontalLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  bool isHorizontal() const override { return true; }
  bool isEmpty() const override { return m_numberOfChildren == 1 && const_cast<HorizontalLayoutNode *>(this)->childAtIndex(0)->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return m_numberOfChildren != 0; }
  bool hasText() const override;

  // TreeNode
  size_t size() const override { return sizeof(HorizontalLayoutNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "HorizontalLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override;
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  // See comment on NAryExpressionNode
  uint16_t m_numberOfChildren;
};

class HorizontalLayout final : public Layout {
  friend class HorizontalLayoutNode;
public:
  // Constructors
  HorizontalLayout(HorizontalLayoutNode * n) : Layout(n) {}
  static HorizontalLayout Builder() { return TreeHandle::NAryBuilder<HorizontalLayout,HorizontalLayoutNode>(); }
  static HorizontalLayout Builder(Layout l) { return HorizontalLayout::Builder(&l, 1); }
  static HorizontalLayout Builder(Layout l1, Layout l2) { return HorizontalLayout::Builder(ArrayBuilder<Layout>(l1, l2).array(), 2); }
  static HorizontalLayout Builder(Layout l1, Layout l2, Layout l3) { return HorizontalLayout::Builder(ArrayBuilder<Layout>(l1, l2, l3).array(), 3); }
  static HorizontalLayout Builder(Layout l1, Layout l2, Layout l3, Layout l4) { return HorizontalLayout::Builder(ArrayBuilder<Layout>(l1, l2, l3, l4).array(), 4); }
  static HorizontalLayout Builder(Layout * children, size_t numberOfChildren) { return TreeHandle::NAryBuilder<HorizontalLayout,HorizontalLayoutNode>(static_cast<TreeHandle *>(children), numberOfChildren); }

  void addChildAtIndex(Layout l, int index, int currentNumberOfChildren, LayoutCursor * cursor, bool removeEmptyChildren = false);
  // Remove puts a child at the end of the pool
  void removeChild(Layout l, LayoutCursor * cursor, bool force = false) {
    Layout::removeChild(l, cursor, force);
  }
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false) {
    Layout::removeChildAtIndex(index, cursor, force);
  }
  void addOrMergeChildAtIndex(Layout l, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);
  void mergeChildrenAtIndex(HorizontalLayout h, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);

private:
  void removeEmptyChildBeforeInsertionAtIndex(int * index, int * currentNumberOfChildren, bool shouldRemoveOnLeft, LayoutCursor * cursor = nullptr);
};

}

#endif
