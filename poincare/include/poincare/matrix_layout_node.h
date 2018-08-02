#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include "layout_reference.h"
#include "grid_layout_node.h"
#include "layout_cursor.h"

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
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "MatrixLayout"; }
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

class MatrixLayoutRef : public LayoutReference {
  friend class MatrixLayoutNode;
public:
  MatrixLayoutRef(TreeNode * n) : LayoutReference(n) {}
  MatrixLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<MatrixLayoutNode>();
    m_identifier = node->identifier();
  }

  MatrixLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3, LayoutRef l4, int numberOfRows, int numberOfColumns) :
    MatrixLayoutRef()
  {
    assert(numberOfRows*numberOfColumns == 4);
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(0);
      static_cast<MatrixLayoutNode *>(node())->setNumberOfColumns(1);
    }
    addChildTreeAtIndex(l1, 0, 0);
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(1);
    }

    addChildTreeAtIndex(l2, 1, 1);
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(2);
    }
    addChildTreeAtIndex(l3, 2, 2);
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(3);
    }
    addChildTreeAtIndex(l4, 3, 3);
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(numberOfRows);
      static_cast<MatrixLayoutNode *>(node())->setNumberOfColumns(numberOfColumns);
    }
  }
  void setNumberOfRows(int count) {
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfRows(count);
    }
  }
  void setNumberOfColumns(int count) {
    if (!(node()->isAllocationFailure())) {
      static_cast<MatrixLayoutNode *>(node())->setNumberOfColumns(count);
    }
  }

  bool hasGreySquares() const {
    if (!(node()->isAllocationFailure())) {
      return static_cast<MatrixLayoutNode *>(node())->hasGreySquares();
    }
    assert(false);
    return true;
  }

  void addGreySquares() {
    if (!(node()->isAllocationFailure())) {
      return static_cast<MatrixLayoutNode *>(node())->addGreySquares();
    }
  }

  void removeGreySquares() {
    if (!(node()->isAllocationFailure())) {
      return static_cast<MatrixLayoutNode *>(node())->removeGreySquares();
    }
  }
};

}

#endif
