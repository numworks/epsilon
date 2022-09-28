#ifndef POINCARE_INTERNAL_EXPRESSION_INTERFACE_H
#define POINCARE_INTERNAL_EXPRESSION_INTERFACE_H

#include "expression_interface.h"

namespace Poincare {

class InternalExpressionInterface : public ExpressionInterface {
public:
  using ExpressionInterface::ExpressionInterface;
  virtual void beautify(TypeBlock * treeBlock) const {}
};

}

#endif
