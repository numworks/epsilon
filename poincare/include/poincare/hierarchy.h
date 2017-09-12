#ifndef POINCARE_HIERARCHY_H
#define POINCARE_HIERARCHY_H

#include <poincare/expression.h>

namespace Poincare {

class Hierarchy : public Expression {
public:
  Hierarchy(int numberOfOperands);
  void swapOperands(int i, int j);
  void replaceOperand(Expression * oldChild, Expression * newChild);
  void removeOperand(Expression * oldChild);
  void sort() override;
protected:
  int m_numberOfOperands;
private:
  virtual Expression ** operands() = 0;
};

}

#endif
