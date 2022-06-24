#ifndef POINCARE_LIST_SEQUENCE_LAYOUT_NODE_H
#define POINCARE_LIST_SEQUENCE_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ListSequenceLayoutNode final : public LayoutNode {
public:
  // Layout
  Type type() const override { return Type::ListSequenceLayout; }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(ListSequenceLayoutNode); }
  int numberOfChildren() const override { return 3; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSequenceLayoutNode";
  }
#endif
   // Tree navigation
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) override;
  Layout XNTLayout(int childIndex = -1) const override;
private:
  constexpr static int k_functionLayoutIndex = 0;
  constexpr static int k_variableLayoutIndex = 1;
  constexpr static int k_upperBoundLayoutIndex = 2;
  LayoutNode * functionLayout() { return childAtIndex(k_functionLayoutIndex); }
  LayoutNode * variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode * upperBoundLayout() { return childAtIndex(k_upperBoundLayoutIndex); }

   // Sizing and positioning
  constexpr static KDCoordinate k_variableHorizontalMargin = 1;
  constexpr static KDCoordinate k_variableBaselineOffset = 2;
  KDSize computeSize(const KDFont * font) override;
  KDCoordinate computeBaseline(const KDFont * font) override;
  KDPoint positionOfChild(LayoutNode * child, const KDFont * font) override;
  KDPoint positionOfVariable(const KDFont * font);
  KDCoordinate variableSlotBaseline(const KDFont * font);
  KDCoordinate bracesWidth(const KDFont * font);
  void render(KDContext * ctx, KDPoint p, const KDFont * font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class ListSequenceLayout final : public LayoutThreeChildren<ListSequenceLayout, ListSequenceLayoutNode> {
public:
  // sequence(f(k), k, 10)
  ListSequenceLayout() = delete;
};

}

#endif

