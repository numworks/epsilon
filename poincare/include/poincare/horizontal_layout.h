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
  int indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex) const override { /* This case is handled directly by the cursor */ assert(false); return -2; }

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
  bool setEmptyVisibility(EmptyRectangle::State state) {
    if (m_emptyVisibility == state) {
      return false;
    }
    m_emptyVisibility = state;
    return numberOfChildren() == 0; // Return true if empty rectangle is displayed
  }

  KDCoordinate baselineBetweenIndexes(int leftIndex, int rightIndex, KDFont::Size font) const;
  KDSize layoutSizeBetweenIndexes(int leftIndex, int rightIndex, KDFont::Size font) const;
  KDRect relativeSelectionRect(int leftIndex, int rightIndex, KDFont::Size font) const;

private:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * l, KDFont::Size font) override;

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

  void addOrMergeChildAtIndex(Layout l, int index);
  void mergeChildrenAtIndex(HorizontalLayout h, int indexπ);
  using Layout::removeChildAtIndexInPlace;
  using Layout::removeChildInPlace;
  using Layout::addChildAtIndexInPlace;

  Layout squashUnaryHierarchyInPlace();

  void serializeChildren(int firstIndex, int lastIndex, char * buffer, int bufferSize);

  KDSize layoutSizeBetweenIndexes(int leftIndex, int rightIndex, KDFont::Size font) const {
    return node()->layoutSizeBetweenIndexes(leftIndex, rightIndex, font);
  }
  KDCoordinate baselineBetweenIndexes(int leftIndex, int rightIndex, KDFont::Size font) const {
    return node()->baselineBetweenIndexes(leftIndex, rightIndex, font);
  }
  KDRect relativeSelectionRect(int leftIndex, int rightIndex, KDFont::Size font) const {
    return node()->relativeSelectionRect(leftIndex, rightIndex, font);
  }

  void setEmptyColor(EmptyRectangle::Color color) { node()->setEmptyColor(color); }
  bool setEmptyVisibility(EmptyRectangle::State state) { return node()->setEmptyVisibility(state); }

private:
  HorizontalLayoutNode * node() const { return static_cast<HorizontalLayoutNode *>(Layout::node()); }
};

}

#endif
