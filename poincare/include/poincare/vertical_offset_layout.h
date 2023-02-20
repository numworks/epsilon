#ifndef POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H
#define POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H

#include <poincare/empty_rectangle.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class VerticalOffsetLayoutNode final : public LayoutNode {
  friend class HigherOrderDerivativeLayoutNode;
public:
  enum class VerticalPosition : uint8_t {
    Subscript = 0,
    Superscript
  };
  enum class HorizontalPosition : uint8_t {
    Prefix = 0,
    Suffix
  };

  VerticalOffsetLayoutNode(VerticalPosition verticalPosition = VerticalPosition::Superscript, HorizontalPosition horizontalPosition = HorizontalPosition::Suffix) :
    LayoutNode(),
    m_emptyBaseVisibility(EmptyRectangle::State::Visible),
    m_verticalPosition(verticalPosition),
    m_horizontalPosition(horizontalPosition)
  {}

  // Layout
  Type type() const override { return Type::VerticalOffsetLayout; }

  // VerticalOffsetLayoutNode
  VerticalPosition verticalPosition() const { return m_verticalPosition; }
  HorizontalPosition horizontalPosition() const { return m_horizontalPosition; }

  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) override;
  DeletionMethod deletionMethodForCursorLeftOfChild(int childIndex) const override;

  // TreeNode
  size_t size() const override { return sizeof(VerticalOffsetLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << (verticalPosition() == VerticalPosition::Subscript ? "Subscript" : "Superscript");
  }
  void logAttributes(std::ostream & stream) const override {
    if (m_horizontalPosition == HorizontalPosition::Prefix) {
      stream << " prefix=\"1\"";
    }
  }
#endif

  bool setEmptyVisibility(EmptyRectangle::State state);

private:
  // LayoutNode
  constexpr static KDCoordinate k_indiceHeight = 10;
  constexpr static KDCoordinate k_separationMargin = 5;

  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;
  bool protectedIsIdenticalTo(Layout l) override;

  LayoutNode * indiceLayout() { return childAtIndex(0); }
  int baseOffsetInParent() const { return m_horizontalPosition == HorizontalPosition::Prefix ? +1 : -1; }
  LayoutNode * baseLayout();

  KDSize baseSize(KDFont::Size font);
  KDCoordinate baseBaseline(KDFont::Size font);

  EmptyRectangle::State m_emptyBaseVisibility;
  /* Since both enums can be represented on one bit each, use the bitfield
   * syntax to let the compiler cram them into one byte instead of two. */
  VerticalPosition m_verticalPosition : 1;
  HorizontalPosition m_horizontalPosition : 1;
};

class VerticalOffsetLayout final : public Layout {
public:
  static VerticalOffsetLayout Builder(Layout l, VerticalOffsetLayoutNode::VerticalPosition verticalPosition, VerticalOffsetLayoutNode::HorizontalPosition horizontalPosition = VerticalOffsetLayoutNode::HorizontalPosition::Suffix);
  VerticalOffsetLayout() = delete;
  VerticalOffsetLayoutNode::VerticalPosition verticalPosition() const { return node()->verticalPosition(); }
  VerticalOffsetLayoutNode::HorizontalPosition horizontalPosition() const { return node()->horizontalPosition(); }
  bool setEmptyVisibility(EmptyRectangle::State state) { return node()->setEmptyVisibility(state); }
  bool isSuffixSuperscript() const { return verticalPosition() == VerticalOffsetLayoutNode::VerticalPosition::Superscript && horizontalPosition() == VerticalOffsetLayoutNode::HorizontalPosition::Suffix; }

private:
  VerticalOffsetLayoutNode * node() const { return static_cast<VerticalOffsetLayoutNode *>(Layout::node()); }
};

}

#endif
