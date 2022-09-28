#ifndef POINCARE_EXPRESSION_INTERFACE_H
#define POINCARE_EXPRESSION_INTERFACE_H

#include "../type_block.h"
#include "../value_block.h"

namespace Poincare {

class ExpressionInterface {
public:
  virtual void basicReduction(TypeBlock * treeBlock) const {}
  virtual float approximate(const TypeBlock * treeBlock) const = 0;
protected:
  // TODO: tidy somewhere else?
  void projectionReduction(TypeBlock * block, TypeBlock * (*PushNode)(), TypeBlock * (*PushInverseNode)()) const;
};

}

#endif
