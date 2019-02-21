#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include <poincare/grid_layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class MatrixLayout;

class MatrixLayoutNode final : public GridLayoutNode {
  friend class Layout;
  friend class MatrixLayout;
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

class MatrixLayout /*final*/ : public GridLayout {
  friend class MatrixLayoutNode;
public:
  MatrixLayout(const MatrixLayoutNode * n) : GridLayout(n) {}
  static MatrixLayout Builder();
  static MatrixLayout Builder(Layout l1, Layout l2, Layout l3, Layout l4) {
    MatrixLayout m = MatrixLayout::Builder();
    m.addChildAtIndexInPlace(l1, 0, 0);
    m.addChildAtIndexInPlace(l2, 1, 1);
    m.addChildAtIndexInPlace(l3, 2, 2);
    m.addChildAtIndexInPlace(l4, 3, 3);
    m.setDimensions(2, 2);
    return m;
  }

  bool hasGreySquares() const { return node()->hasGreySquares(); }
  void addGreySquares() { node()->addGreySquares(); }
  void removeGreySquares() { node()->removeGreySquares(); }
private:
  MatrixLayoutNode * node() const { return static_cast<MatrixLayoutNode *>(Layout::node()); }
};

}

#endif
