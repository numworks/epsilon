#ifndef POINCARE_HIERARCHY_H
#define POINCARE_HIERARCHY_H

#include <poincare/expression.h>

namespace Poincare {

class Hierarchy : public Expression {
public:
  const Expression * operand(int i) const override;
  void swapOperands(int i, int j) override;
  void replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand = true) override;
  void detachOperands(); // Removes all operands WITHOUT deleting them
  virtual const Expression * const * operands() const = 0;
};

}

#endif
