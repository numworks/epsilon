#ifndef POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H
#define POINCARE_VERTICAL_OFFSET_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class VerticalOffsetLayoutNode final : public LayoutNode {
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
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool mustHaveLeftSibling() const override { return true; }
  bool isVerticalOffset() const override { return true; }
  bool canBeOmittedMultiplicationRightFactor() const override { return false; }

  // TreeNode
  size_t size() const override { return sizeof(VerticalOffsetLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << (m_type == Type::Subscript ? "Subscript" : "Superscript");
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_indiceHeight = 5;
  constexpr static KDCoordinate k_separationMargin = 5;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  LayoutNode * indiceLayout() { return childAtIndex(0); }
  LayoutNode * baseLayout();
  Type m_type;
};

class VerticalOffsetLayout final : public Layout {
public:
  VerticalOffsetLayout(Layout l, VerticalOffsetLayoutNode::Type type);
};

}

#endif
