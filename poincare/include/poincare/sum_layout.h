#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/sequence_layout.h>

namespace Poincare {

class SumLayoutNode final : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;

  // Layout
  Type type() const override { return Type::SumLayout; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(SumLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "SumLayout";
  }
#endif

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class SumLayout final : public Layout {
public:
  static SumLayout Builder(Layout argument, Layout variable, Layout lowerB, Layout upperB)  { return TreeHandle::FixedArityBuilder<SumLayout,SumLayoutNode>(ArrayBuilder<TreeHandle>(argument, variable, lowerB, upperB).array(), 4); }
  SumLayout() = delete;
};

}

#endif
