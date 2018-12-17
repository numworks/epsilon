#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/sequence_layout.h>

namespace Poincare {

class SumLayoutNode final : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
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
  SumLayout(Layout argument, Layout variable, Layout lowerB, Layout upperB) :
    Layout(TreePool::sharedPool()->createTreeNode<SumLayoutNode>())
  {
    replaceChildAtIndexInPlace(0, argument);
    replaceChildAtIndexInPlace(1, variable);
    replaceChildAtIndexInPlace(2, lowerB);
    replaceChildAtIndexInPlace(3, upperB);
  }
};

}

#endif
