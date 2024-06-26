#ifndef POINCARE_MATRIX_LAYOUT_NODE_H
#define POINCARE_MATRIX_LAYOUT_NODE_H

#include "grid_layout.h"
#include "horizontal_layout.h"
#include "layout_cursor.h"
#include "old_layout.h"

namespace Poincare {

class MatrixLayout;

class MatrixLayoutNode final : public GridLayoutNode {
  friend class MatrixLayout;

 public:
  using GridLayoutNode::GridLayoutNode;

  // Layout
  Type otype() const override { return Type::MatrixLayout; }

  // SerializableNode
  size_t serialize(char *buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Grid layout node
  void startEditing() override;
  void stopEditing() override;

  // PoolObject
  size_t size() const override { return sizeof(MatrixLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override {
    stream << "MatrixLayout";
  }
#endif

 private:
  // Grid layout node
  bool isEditing() const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode *l, KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) override;
};

class MatrixLayout final : public GridLayout {
  friend class MatrixLayoutNode;

 public:
  MatrixLayout(const MatrixLayoutNode *n) : GridLayout(n) {}
  static MatrixLayout Builder() {
    return PoolHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>();
  }
  static MatrixLayout Builder(OLayout l1);
  static MatrixLayout Builder(OLayout l1, OLayout l2, OLayout l3, OLayout l4);
  static MatrixLayout EmptyMatrixBuilder() {
    return Builder(HorizontalLayout::Builder());
  }

 private:
  MatrixLayoutNode *node() const {
    return static_cast<MatrixLayoutNode *>(OLayout::object());
  }
};

}  // namespace Poincare

#endif
