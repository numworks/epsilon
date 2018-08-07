#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/sequence_layout_node.h>

namespace Poincare {

class SumLayoutNode : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class SumLayoutRef : public LayoutReference {
public:
  SumLayoutRef(LayoutRef argument, LayoutRef lowerB, LayoutRef upperB) :
    LayoutReference(TreePool::sharedPool()->createTreeNode<SumLayoutNode>(), true)
  {
    replaceChildAtIndexInPlace(0, argument);
    replaceChildAtIndexInPlace(1, lowerB);
    replaceChildAtIndexInPlace(2, upperB);
  }
};

}

#endif
