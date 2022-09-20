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

  // Layout
  Type type() const override { return Type::MatrixLayout; }

  // MatrixLayoutNode
  void addGraySquares();
  void removeGraySquares();

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void willAddSiblingToEmptyChildAtIndex(int childIndex) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // TreeNode
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * l, KDFont::Size font) override;
  void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) override;

private:
  // MatrixNode
  void newRowOrColumnAtIndex(int index);
  bool isRowEmpty(int index) const;
  bool isColumnEmpty(int index) const;
  bool hasGraySquares() const;

  // LayoutNode
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
};

class MatrixLayout final : public GridLayout {
  friend class MatrixLayoutNode;
public:
  MatrixLayout(const MatrixLayoutNode * n) : GridLayout(n) {}
  static MatrixLayout Builder() { return TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>(); }
  static MatrixLayout Builder(Layout l1, Layout l2, Layout l3, Layout l4);

  bool hasGraySquares() const { return node()->hasGraySquares(); }
  void addGraySquares() { node()->addGraySquares(); }
  void removeGraySquares() { node()->removeGraySquares(); }
private:
  MatrixLayoutNode * node() const { return static_cast<MatrixLayoutNode *>(Layout::node()); }
};

}

#endif
