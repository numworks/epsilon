#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

/* WARNING: A HorizontalLayout should never have a HorizontalLayout child. For
 * instance, use addOrMergeChildAtIndex to add a LayoutNode safely. */
class HorizontalLayout;

class HorizontalLayoutNode final : public LayoutNode {
  friend class Layout;
  friend class HorizontalLayout;
public:

  HorizontalLayoutNode() :
    LayoutNode(),
    m_numberOfChildren(0)
  {}

  // Layout
  Type type() const override { return Type::HorizontalLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression, bool * forceCursorLeftOfText = nullptr) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  int serializeChildrenBetweenIndexes(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, bool forceIndexes, int firstIndex = -1, int lastIndex = -1) const;

  bool isEmpty() const override { return m_numberOfChildren == 1 && const_cast<HorizontalLayoutNode *>(this)->childAtIndex(0)->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return m_numberOfChildren != 0; }
  bool hasText() const override;

  // TreeNode
  size_t size() const override { return sizeof(HorizontalLayoutNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override { m_numberOfChildren = numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "HorizontalLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * l, KDFont::Size font) override;
  KDRect relativeSelectionRect(const Layout * selectionStart, const Layout * selectionEnd, KDFont::Size font) const;

private:
  bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override;
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  // See comment on NAryExpressionNode
  uint16_t m_numberOfChildren;
};

class HorizontalLayout final : public Layout {
  friend class HorizontalLayoutNode;
public:
  // Constructors
  HorizontalLayout(HorizontalLayoutNode * n) : Layout(n) {}

  // FIXME: use Layout instead of TreeHandle
  static HorizontalLayout Builder(std::initializer_list<TreeHandle> children = {}) { return TreeHandle::NAryBuilder<HorizontalLayout,HorizontalLayoutNode>(children); }
  // TODO: Get rid of those helpers
  static HorizontalLayout Builder(Layout l) { return Builder({l}); }
  static HorizontalLayout Builder(Layout l1, Layout l2) { return Builder({l1, l2}); }
  static HorizontalLayout Builder(Layout l1, Layout l2, Layout l3) { return Builder({l1, l2, l3}); }
  static HorizontalLayout Builder(Layout l1, Layout l2, Layout l3, Layout l4) { return Builder({l1, l2, l3, l4}); }

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

  Layout squashUnaryHierarchyInPlace();

  void serializeChildren(int firstIndex, int lastIndex, char * buffer, int bufferSize);

  KDRect relativeSelectionRect(const Layout * selectionStart, const Layout * selectionEnd, KDFont::Size font) const { return static_cast<HorizontalLayoutNode *>(node())->relativeSelectionRect(selectionStart, selectionEnd, font); }
private:
  void removeEmptyChildBeforeInsertionAtIndex(int * index, int * currentNumberOfChildren, bool shouldRemoveOnLeft, bool shouldRemoveOnRight, LayoutCursor * cursor = nullptr);
};

}

#endif
