#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include "layout_reference.h"
#include "grid_layout_node.h"
#include "layout_cursor.h"

namespace Poincare {

class MatrixLayoutNode : public GridLayoutNode {
  template <typename T>
  friend class LayoutReference;
public:
  using GridLayoutNode::GridLayoutNode;

  // MatrixLayoutNode
  void newRowOrColumnAtIndex(int index);
  void addGreySquares();
  void removeGreySquares();

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool isMatrix() const override { return true; }

  // SerializableNode
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if TREE_LOG
  const char * description() const override {
    return "MatrixLayout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  KDPoint positionOfChild(LayoutNode * l) override;
  void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override;

private:
  // MatrixNode
  void childWasReplacedAtIndex(int index);
  bool isRowEmpty(int index) const;
  bool isColumnEmpty(int index) const;
  bool hasGreySquares() const;

  // LayoutNode
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
};

class MatrixLayoutRef : public LayoutReference<MatrixLayoutNode> {
  friend class MatrixLayoutNode;
public:
  MatrixLayoutRef(TreeNode * n) : LayoutReference<MatrixLayoutNode>(n) {}
  MatrixLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3, LayoutRef l4, int numberOfRows, int numberOfColumns) :
    LayoutReference<MatrixLayoutNode>()
  {
    if (!(node()->isAllocationFailure())) {
      assert(numberOfRows*numberOfColumns == 4);
      typedNode()->setNumberOfRows(numberOfRows);
      typedNode()->setNumberOfColumns(numberOfColumns);
    }
    addChildTreeAtIndex(l1, 0);
    addChildTreeAtIndex(l2, 1);
    addChildTreeAtIndex(l3, 2);
    addChildTreeAtIndex(l4, 3);
  }
};

}

#endif
