#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include <poincare/grid_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class MatrixLayout;

class MatrixLayoutNode final : public GridLayoutNode {
  friend class MatrixLayout;
public:
  using GridLayoutNode::GridLayoutNode;

  // Layout
  Type type() const override { return Type::MatrixLayout; }

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Grid layout node
  void startEditing() override;
  void stopEditing() override;

  // TreeNode
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixLayout";
  }
#endif

private:
  // Grid layout node
  bool isEditing() const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * l, KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;

};

class MatrixLayout final : public GridLayout {
  friend class MatrixLayoutNode;
public:
  MatrixLayout(const MatrixLayoutNode * n) : GridLayout(n) {}
  static MatrixLayout Builder() { return TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>(); }
  static MatrixLayout Builder(Layout l1);
  static MatrixLayout Builder(Layout l1, Layout l2, Layout l3, Layout l4);
  static MatrixLayout EmptyMatrixBuilder() { return Builder(HorizontalLayout::Builder()); }

private:
  MatrixLayoutNode * node() const { return static_cast<MatrixLayoutNode *>(Layout::node()); }
};

}

#endif
