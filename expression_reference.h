#ifndef EXPRESSION_REFERENCE_H
#define EXPRESSION_REFERENCE_H

#include "tree_reference.h"
#include "expression_node.h"

#include <stdio.h>

template <class T>
class ExpressionReference : public TreeReference<T> {
public:
  ExpressionReference() : TreeReference<T>(ExpressionNode::Pool()) { }

  /*ExpressionReference(const ExpressionReference & er) {
  }*/

  // Allow every ExpressionReference<T> to be transformed into an ExpressionReference<ExpressionNode>, i.e. Expression
  operator ExpressionReference<ExpressionNode>() const {
    printf("ExpressionReference cast\n");
    // TODO: make sure this is kosher
    // static_assert(sizeof(ExpressionReference<T>) == sizeof(ExpressionReference<ExpressionNode>), "All ExpressionReference are supposed to have the same size");
    return *(reinterpret_cast<const ExpressionReference<ExpressionNode> *>(this));
  }

  float approximate() const {
    return this->node()->approximate();
  }

  /*
  ExpressionReference<ExpressionNode> simplify() {
    return node()->simplify();
  }
  */
};

typedef ExpressionReference<ExpressionNode> Expression;

#endif
