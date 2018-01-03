#ifndef POINCARE_EXPRESSION_ARRAY_H
#define POINCARE_EXPRESSION_ARRAY_H

#include <poincare/expression.h>

namespace Poincare {

class ExpressionArray {
public:
  ExpressionArray(const Expression * eL1 = nullptr, const Expression * eL2 = nullptr) :
    m_data{eL1, eL2}
  {}
  const Expression * const * array() { return const_cast<const Expression * const *>(m_data); }
private:
  const Expression * m_data[2];
};

}

#endif
