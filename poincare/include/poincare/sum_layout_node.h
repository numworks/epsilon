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

class SumLayoutRef : public LayoutReference {
public:
  SumLayoutRef(TreeNode * t) : LayoutReference(t) {}
  SumLayoutRef(LayoutRef argument, LayoutRef lowerB, LayoutRef upperB) :
    SumLayoutRef()
  {
    addChildTreeAtIndex(argument, 0);
    addChildTreeAtIndex(lowerB, 1);
    addChildTreeAtIndex(upperB, 2);
  }
private:
  SumLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<SumLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
