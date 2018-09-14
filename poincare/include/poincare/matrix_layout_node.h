#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include <poincare/grid_layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class MatrixLayoutReference;

class MatrixLayoutNode : public GridLayoutNode {
  friend class LayoutReference;
  friend class MatrixLayoutReference;
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

class MatrixLayoutReference : public GridLayoutReference {
  friend class MatrixLayoutNode;
public:
  MatrixLayoutReference(const MatrixLayoutNode * n);
  MatrixLayoutReference();
  MatrixLayoutReference(LayoutReference l1, LayoutReference l2, LayoutReference l3, LayoutReference l4);
  bool hasGreySquares() const { return node()->hasGreySquares(); }
  void addGreySquares() { node()->addGreySquares(); }
  void removeGreySquares() { node()->removeGreySquares(); }
private:
  MatrixLayoutNode * node() const { return static_cast<MatrixLayoutNode *>(LayoutReference::node()); }
};

}

#endif
