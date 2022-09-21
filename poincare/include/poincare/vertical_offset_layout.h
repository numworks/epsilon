#ifndef POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H
#define POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class VerticalOffsetLayoutNode final : public LayoutNode {
  friend class HigherOrderDerivativeLayoutNode;
public:
  enum class Position {
    Subscript,
    Superscript
  };

  VerticalOffsetLayoutNode(Position position = Position::Superscript) :
    LayoutNode(),
    m_position(position)
  {}

  // Layout
  Type type() const override { return Type::VerticalOffsetLayout; }

  // VerticalOffsetLayoutNode
  Position position() const { return m_position; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool mustHaveLeftSibling() const override { return true; }
  bool canBeOmittedMultiplicationRightFactor() const override { return false; }

  // TreeNode
  size_t size() const override { return sizeof(VerticalOffsetLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << (m_position == Position::Subscript ? "Subscript" : "Superscript");
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
private:
  constexpr static KDCoordinate k_indiceHeight = 10;
  constexpr static KDCoordinate k_separationMargin = 5;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override {}
  bool protectedIsIdenticalTo(Layout l) override;
  LayoutNode * indiceLayout() { return childAtIndex(0); }
  LayoutNode * baseLayout();
  Position m_position;
};

class VerticalOffsetLayout final : public Layout {
public:
  static VerticalOffsetLayout Builder(Layout l, VerticalOffsetLayoutNode::Position position);
  VerticalOffsetLayout() = delete;
  VerticalOffsetLayoutNode::Position position() const { return static_cast<VerticalOffsetLayoutNode *>(node())->position(); }
};

}

#endif
