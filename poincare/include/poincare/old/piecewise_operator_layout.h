#ifndef POINCARE_PIECEWISE_OPERATOR_LAYOUT_NODE_H
#define POINCARE_PIECEWISE_OPERATOR_LAYOUT_NODE_H

#include "grid_layout.h"
#include "layout_cursor.h"
#include "old_layout.h"

namespace Poincare {

class PiecewiseOperatorLayoutNode final : public GridLayoutNode {
  friend class PiecewiseOperatorLayout;

 public:
  using GridLayoutNode::GridLayoutNode;

  // Layout
  Type otype() const override { return Type::PiecewiseOperatorLayout; }

  // SerializableNode
  size_t serialize(char *buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Grid layout node
  void startEditing() override;
  void stopEditing() override;

  // PoolObject
  size_t size() const override { return sizeof(PiecewiseOperatorLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override {
    stream << "PiecewiseOperatorLayout";
  }
#endif

 private:
  // Grid layout node
  KDCoordinate horizontalGridEntryMargin(KDFont::Size font) const override {
    return 2 * k_gridEntryMargin + KDFont::GlyphWidth(font);
  }
  bool numberOfColumnsIsFixed() const override { return true; }
  bool isEditing() const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode *l, KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) override;

  void makeLastConditionVisibleIfEmpty(bool visible);
};

class PiecewiseOperatorLayout /*final*/ : public GridLayout {
  friend class PiecewiseOperatorLayoutNode;

 public:
  PiecewiseOperatorLayout(const PiecewiseOperatorLayoutNode *n)
      : GridLayout(n) {}
  static PiecewiseOperatorLayout Builder();
  void addRow(OLayout leftLayout, OLayout rightLayout = OLayout());

 private:
  // Make private. Use addRow instead.
  using GridLayout::addChildAtIndexInPlace;
  PiecewiseOperatorLayoutNode *node() const {
    return static_cast<PiecewiseOperatorLayoutNode *>(OLayout::object());
  }
};

}  // namespace Poincare

#endif
