#ifndef POINCARE_CONDENSED_SUM_LAYOUT_NODE_H
#define POINCARE_CONDENSED_SUM_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/sum.h>

namespace Poincare {

class CondensedSumLayoutNode final : public LayoutNode {
 public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::CondensedSumLayout; }

  /* CondensedSumLayout is only used in apps/shared/sum_graph_controller.cpp, in
   * a view with no cursor. */

  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
  }

  int indexOfChildToPointToWhenInserting() override {
    assert(false);
    return k_outsideIndex;
  }

  // TreeNode
  size_t size() const override { return sizeof(CondensedSumLayoutNode); }
  int numberOfChildren() const override { return 3; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "CondensedSumLayout";
  }
#endif

 protected:
  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;

 private:
  void render(KDContext* ctx, KDPoint p, KDFont::Size font,
              KDColor expressionColor, KDColor backgroundColor) override {}
  LayoutNode* baseLayout() { return childAtIndex(0); }
  LayoutNode* subscriptLayout() { return childAtIndex(1); }
  LayoutNode* superscriptLayout() { return childAtIndex(2); }
};

class CondensedSumLayout final
    : public LayoutThreeChildren<CondensedSumLayout, CondensedSumLayoutNode> {
 public:
  CondensedSumLayout() = delete;
};

}  // namespace Poincare

#endif
