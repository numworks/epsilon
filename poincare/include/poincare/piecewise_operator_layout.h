#ifndef POINCARE_PIECEWISE_OPERATOR_LAYOUT_NODE_H
#define POINCARE_PIECEWISE_OPERATOR_LAYOUT_NODE_H

#include <poincare/grid_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class PiecewiseOperatorLayoutNode final : public GridLayoutNode {
  friend class PiecewiseOperatorLayout;

 public:
  using GridLayoutNode::GridLayoutNode;

  // Layout
  Type type() const override { return Type::PiecewiseOperatorLayout; }

  // SerializableNode
  int serialize(char *buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  // Grid layout node
  void startEditing() override;
  void stopEditing() override;

  // TreeNode
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
  void render(KDContext *ctx, KDPoint p, KDFont::Size font,
              KDColor expressionColor, KDColor backgroundColor) override;

  void makeLastConditionVisibleIfEmpty(bool visible);
};

class PiecewiseOperatorLayout /*final*/ : public GridLayout {
  friend class PiecewiseOperatorLayoutNode;

 public:
  PiecewiseOperatorLayout(const PiecewiseOperatorLayoutNode *n)
      : GridLayout(n) {}
  static PiecewiseOperatorLayout Builder();
  void addRow(Layout leftLayout, Layout rightLayout = Layout());

 private:
  using GridLayout::addChildAtIndexInPlace;  // Make private. Use addRow
                                             // instead.
  PiecewiseOperatorLayoutNode *node() const {
    return static_cast<PiecewiseOperatorLayoutNode *>(Layout::node());
  }
};

}  // namespace Poincare

#endif
