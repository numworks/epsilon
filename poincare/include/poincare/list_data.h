#ifndef POINCARE_LIST_DATA_H
#define POINCARE_LIST_DATA_H

#include <poincare/expression.h>

namespace Poincare {

class ListData {
public:
  ListData(Expression * operand);
  ~ListData();
  int numberOfOperands() const;
  const Expression * operand(int i) const;
  void pushExpression(Expression * operand);
private:
  int m_numberOfOperands;
  Expression ** m_operands;
};

}

#endif
