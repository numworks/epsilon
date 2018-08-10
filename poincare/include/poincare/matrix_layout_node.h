#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include <poincare/allocation_failure_layout_node.h>
#include <poincare/grid_layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class MatrixLayoutRef;

class MatrixLayoutNode : public GridLayoutNode {
  friend class LayoutReference;
  friend class MatrixLayoutRef;
public:
  using GridLayoutNode::GridLayoutNode;

  // MatrixLayoutNode
  void addGreySquares();
  void removeGreySquares();

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void willAddSiblingToEmptyChildAtIndex(int childIndex) override;
  bool isMatrix() const override { return true; }

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  static MatrixLayoutNode * FailedAllocationStaticNode();
  MatrixLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "MatrixLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDPoint positionOfChild(LayoutNode * l) override;
  void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor,  bool * shouldRecomputeLayout, bool equivalentPositionVisited) override;

private:
  // MatrixNode
  void newRowOrColumnAtIndex(int index);
  bool isRowEmpty(int index) const;
  bool isColumnEmpty(int index) const;
  bool hasGreySquares() const;

  // LayoutNode
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
};

class AllocationFailureMatrixLayoutNode : public AllocationFailureLayoutNode<MatrixLayoutNode> {
  void setNumberOfRows(int numberOfRows) override {}
  void setNumberOfColumns(int numberOfColumns) override {}
};

class MatrixLayoutRef : public LayoutReference {
  friend class MatrixLayoutNode;
public:
  MatrixLayoutRef(const MatrixLayoutNode * n) : LayoutReference(n) {}
  MatrixLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<MatrixLayoutNode>()) {}
  MatrixLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3, LayoutRef l4) :
    MatrixLayoutRef()
  {
    addChildAtIndexInPlace(l1, 0, 0);
    addChildAtIndexInPlace(l2, 1, 1);
    addChildAtIndexInPlace(l3, 2, 2);
    addChildAtIndexInPlace(l4, 3, 3);
    setDimensions(2, 2);
  }
  bool hasGreySquares() const { return node()->hasGreySquares(); }
  void addGreySquares() { node()->addGreySquares(); }
  void removeGreySquares() { node()->removeGreySquares(); }
  void setDimensions(int rows, int columns);
  //void addChildAtIndexInPlace(TreeByReference t, int index, int currentNumberOfChildren) override;
  void addChildAtIndex(LayoutReference l, int index, int currentNumberOfChildren, LayoutCursor * cursor) override {
    LayoutReference::addChildAtIndex(l, index, currentNumberOfChildren, cursor);
  }
private:
  MatrixLayoutNode * node() const { return static_cast<MatrixLayoutNode *>(LayoutReference::node()); }
  void setNumberOfRows(int rows) {
    assert(rows >= 0);
    node()->setNumberOfRows(rows);
  }
  void setNumberOfColumns(int columns) {
    assert(columns >= 0);
    node()->setNumberOfColumns(columns);
  }
};

}

#endif
