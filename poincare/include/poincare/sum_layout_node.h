#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include <poincare/layout_engine.h>
#include <poincare/sequence_layout_node.h>

namespace Poincare {

class SumLayoutNode : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class SumLayoutRef : public LayoutReference<SumLayoutNode> {
public:
  SumLayoutRef(TreeNode * n) : LayoutReference<SumLayoutNode>(n) {}
  SumLayoutRef(LayoutRef argument, LayoutRef lowerB, LayoutRef upperB) : LayoutReference<SumLayoutNode>() {
    addChildTreeAtIndex(argument, 0);
    addChildTreeAtIndex(lowerB, 1);
    addChildTreeAtIndex(upperB, 2);
  }
};

}

#endif
