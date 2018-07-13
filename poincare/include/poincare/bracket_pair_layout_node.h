#ifndef POINCARE_BRACKET_PAIR_LAYOUT_NODE_H
#define POINCARE_BRACKET_PAIR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class BracketPairLayoutNode : public LayoutNode {
  //TODO friend class MatrixLayout;
public:
  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override{ return true; }
  void didCollapseSiblings(LayoutCursor * cursor) override;

  // TreeNode
  size_t size() const override { return sizeof(BracketPairLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if TREE_LOG
  const char * description() const override { return "BracketPairLayout"; }
#endif

protected:
  // LayoutNode
  void computeBaseline() override;
  void computeSize() override;
  KDPoint positionOfChild(LayoutNode * child) override;
  LayoutNode * childLayout() { return childAtIndex(0); }

private:
  KDCoordinate externWidthMargin() const { return 2; }
  virtual KDCoordinate widthMargin() const { return 5; }
  virtual KDCoordinate verticalExternMargin() const { return 0; }
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 1;
  virtual bool renderTopBar() const { return true; }
  virtual bool renderBottomBar() const { return true; }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif

