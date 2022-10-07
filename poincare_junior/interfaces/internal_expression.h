#ifndef POINCARE_INTERFACES_INTERNAL_EXPRESSION_H
#define POINCARE_INTERFACES_INTERNAL_EXPRESSION_H

#include "expression.h"

namespace Poincare {

class InternalExpressionInterface : public ExpressionInterface {
public:
  using ExpressionInterface::ExpressionInterface;
  virtual void beautify(TypeBlock * treeBlock) const {}
};

}

#endif
