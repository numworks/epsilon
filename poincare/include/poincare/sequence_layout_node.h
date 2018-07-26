#ifndef POINCARE_SEQUENCE_LAYOUT_NODE_H
#define POINCARE_SEQUENCE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class SequenceLayoutNode : public LayoutNode {
public:
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;

  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting() override {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout();
  }
  char XNTChar() const override { return 'n'; }

  // TreeNode
  size_t size() const override { return sizeof(SequenceLayoutNode); }
#if TREE_LOG
  const char * description() const override {
    return "Sequence Layout";
  }
#endif

protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  constexpr static KDText::FontSize k_fontSize = KDText::FontSize::Large;

  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;

  int writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const;
  LayoutNode * argumentLayout() {
    assert(numberOfChildren() == 3);
    return childAtIndex(0);
  }
  LayoutNode * lowerBoundLayout() {
    assert(numberOfChildren() == 3);
    return childAtIndex(1);
  }
  LayoutNode * upperBoundLayout() {
    assert(numberOfChildren() == 3);
    return childAtIndex(2);
  }

  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  KDSize lowerBoundSizeWithNEquals();
};

/*class SequenceLayoutRef : public LayoutReference<SequenceLayoutNode> {
public:
  VerticalOffsetLayoutRef(LayoutRef l, VerticalOffsetLayoutNode::Type type) :
    LayoutReference<VerticalOffsetLayoutNode>()
  {
    if (!(this->node()->isAllocationFailure())) {
      this->typedNode()->setType(type);
    }
    addChildTreeAtIndex(l, 0);
  }
  VerticalOffsetLayoutRef(TreeNode * t) : LayoutReference<VerticalOffsetLayoutNode>(t) {}
};*/

}

#endif
