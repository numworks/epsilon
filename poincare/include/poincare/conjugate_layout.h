#ifndef POINCARE_CONJUGATE_LAYOUT_NODE_H
#define POINCARE_CONJUGATE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ConjugateLayoutNode : public LayoutNode {
 public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::ConjugateLayout; }

  // LayoutNode
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;

  // TreeNode
  size_t size() const override { return sizeof(ConjugateLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ConjugateLayout";
  }
#endif

 protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineVerticalMargin = 1;
  LayoutNode* childLayout() { return childAtIndex(0); }

 private:
  void render(KDContext* ctx, KDPoint p, KDFont::Size font,
              KDColor expressionColor, KDColor backgroundColor) override;
};

class ConjugateLayout final
    : public LayoutOneChild<ConjugateLayout, ConjugateLayoutNode> {
 public:
  ConjugateLayout() = delete;
};

}  // namespace Poincare

#endif
