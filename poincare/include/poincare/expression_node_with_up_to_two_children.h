#ifndef POINCARE_EXPRESSION_NODE_WITH_UP_TO_TWO_CHILDREN_H
#define POINCARE_EXPRESSION_NODE_WITH_UP_TO_TWO_CHILDREN_H

#include <poincare/expression_node.h>

namespace Poincare {

class ExpressionNodeWithOneOrTwoChildren : public ExpressionNode {
 public:
  int numberOfChildren() const override { return m_hasTwoChildren ? 2 : 1; }
  void setNumberOfChildren(int numberOfChildren) override {
    if (numberOfChildren == 1 || numberOfChildren <= 2) {
      m_hasTwoChildren = numberOfChildren == 2;
    }
  }

 private:
  bool m_hasTwoChildren;
};

}  // namespace Poincare

#endif
