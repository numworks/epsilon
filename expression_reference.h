#ifndef EXPRESSION_REFERENCE_H
#define EXPRESSION_REFERENCE_H

#include "tree_reference.h"
#include "expression_node.h"

#include <stdio.h>

template <class T>
class ExpressionReference : public TreeReference<T> {
public:
  using TreeReference<T>::TreeReference;

  // Allow every ExpressionReference<T> to be transformed into an ExpressionReference<ExpressionNode>, i.e. Expression
  operator ExpressionReference<ExpressionNode>() const {
    return ExpressionReference<ExpressionNode>(this->node());
  }

  static TreeNode * failedAllocationNode();

  void addChild(ExpressionReference<ExpressionNode> e) {
    if (!this->node()->isAllocationFailure()) {
      TreeReference<T>::addTreeChild(e);
    }
  }

  ExpressionReference<ExpressionNode> childAtIndex(int i) {
    return ExpressionReference<ExpressionNode>(TreeReference<T>::treeChildAtIndex(i).node());
  }

  void replaceChildAtIndex(int oldChildIndex, ExpressionReference<ExpressionNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }

  float approximate() const {
    return this->castedNode()->approximate();
  }

  /*
  ExpressionReference<ExpressionNode> simplify() {
    return node()->simplify();
  }
  */
};

typedef ExpressionReference<ExpressionNode> ExpressionRef;

#endif
