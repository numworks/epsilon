#ifndef NCR_LAYOUT_NODE_H
#define NCR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>
#include <poincare/grid_layout.h>
#include <algorithm>

namespace Poincare {

class NCRLayoutNode final : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::NCRLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(NCRLayoutNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NCRLayout";
  }
#endif

protected:
  // LayoutNode
  static constexpr KDCoordinate k_margin = 3;
  static constexpr KDCoordinate k_symbolHeight = 16;
  static constexpr KDCoordinate k_symbolBaseline = 11;
  static constexpr KDCoordinate k_symbolWidth = 12;
  static constexpr KDCoordinate k_symbolWidthWithMargins = k_symbolWidth + 2 * k_margin;
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  KDCoordinate aboveSymbol();
  KDCoordinate totalHeight();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  LayoutNode * nLayout() { return childAtIndex(0); }
  LayoutNode * kLayout() { return childAtIndex(1); }
};

class NCRLayout final : public Layout {
public:
  static NCRLayout Builder(Layout child0, Layout child1) { return TreeHandle::FixedArityBuilder<NCRLayout, NCRLayoutNode>({child0, child1}); }
  NCRLayout() = delete;
};

}

#endif
