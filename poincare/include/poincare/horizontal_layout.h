#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include <poincare/empty_rectangle.h>
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
    m_numberOfChildren(0),
    m_emptyColor(EmptyRectangle::Color::Yellow),
    m_emptyVisibility(EmptyRectangle::State::Visible)
  {}

  // Layout
  Type type() const override { return Type::HorizontalLayout; }

  // LayoutNode
  void moveCursorHorizontally(OMG::NewHorizontalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout);
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression, bool * forceCursorLeftOfText = nullptr) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  int serializeChildrenBetweenIndexes(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, bool forceIndexes, int firstIndex = -1, int lastIndex = -1) const;

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

  void setEmptyColor(EmptyRectangle::Color color) { m_emptyColor = color; }
  void setEmptyVisibility(EmptyRectangle::State state) { m_emptyVisibility = state; }

  KDRect relativeSelectionRect(int leftIndex, int rightIndex, KDFont::Size font) const;

private:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * l, KDFont::Size font) override;

  bool willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;

  bool shouldDrawEmptyRectangle() const;

  // See comment on NAryExpressionNode
  uint16_t m_numberOfChildren;

  EmptyRectangle::Color m_emptyColor;
  EmptyRectangle::State m_emptyVisibility;
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

  void addOrMergeChildAtIndex(Layout l, int index, LayoutCursor * cursor = nullptr);
  void mergeChildrenAtIndex(HorizontalLayout h, int index, LayoutCursor * cursor = nullptr);
  using Layout::removeChild;
  using Layout::removeChildAtIndex;
  using Layout::addChildAtIndex;

  Layout squashUnaryHierarchyInPlace();

  void serializeChildren(int firstIndex, int lastIndex, char * buffer, int bufferSize);

  KDRect relativeSelectionRect(const Layout * selectionStart, const Layout * selectionEnd, KDFont::Size font) const {/* TODO */ return KDRectZero; /* node()->relativeSelectionRect(selectionStart, selectionEnd, font);*/ }

  void setEmptyColor(EmptyRectangle::Color color) { node()->setEmptyColor(color); }
  void setEmptyVisibility(EmptyRectangle::State state) { node()->setEmptyVisibility(state); }

private:
  HorizontalLayoutNode * node() const { return static_cast<HorizontalLayoutNode *>(Layout::node()); }
};

}

#endif
