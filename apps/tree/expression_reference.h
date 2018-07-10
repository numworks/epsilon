#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/expression_node.h>
#include <poincare/serializable_reference.h>

#include <stdio.h>

template <class T>
class ExpressionReference : public SerializableReference<T> {
public:
  using SerializableReference<T>::SerializableReference;

  /* Allow every ExpressionReference<T> to be transformed into an
   * ExpressionReference<ExpressionNode>, i.e. ExpressionRef */
  operator ExpressionReference<ExpressionNode>() const {
    return ExpressionReference<ExpressionNode>(this->node());
  }

  static TreeNode * FailedAllocationStaticNode();

  ExpressionReference<ExpressionNode> childAtIndex(int i) {
    return ExpressionReference<ExpressionNode>(TreeReference<T>::treeChildAtIndex(i).node());
  }

  void replaceChildAtIndex(int oldChildIndex, ExpressionReference<ExpressionNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }

  float approximate() const {
    return this->typedNode()->approximate();
  }

  ExpressionReference<ExpressionNode> deepReduce() {
    ExpressionReference<ExpressionNode> result = ExpressionReference<ExpressionNode>(this->clone().node());
    result.typedNode()->deepReduce();
    return result;
  }

  void shallowReduce() {
    return this->typedNode()->shallowReduce();
  }

  void sortChildren() {
    for (int i = this->numberOfChildren()-1; i > 0; i--) {
      bool isSorted = true;
      for (int j = 0; j < this->numberOfChildren()-1; j++) {
        if (this->childAtIndex(j).typedNode()->type() > this->childAtIndex(j+1).typedNode()->type()) {
          this->swapChildren(j, j+1);
          isSorted = false;
        }
      }
      if (isSorted) {
        return;
      }
    }
  }


};

typedef ExpressionReference<ExpressionNode> ExpressionRef;

#endif
