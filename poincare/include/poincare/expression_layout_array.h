#ifndef POINCARE_EXPRESSION_LAYOUT_ARRAY_H
#define POINCARE_EXPRESSION_LAYOUT_ARRAY_H

#include <poincare/expression_layout.h>

namespace Poincare {

class ExpressionLayoutArray {
public:
  ExpressionLayoutArray(const ExpressionLayout * eL1 = nullptr, const ExpressionLayout * eL2 = nullptr, const ExpressionLayout * eL3 = nullptr) :
    m_data{eL1, eL2, eL3}
  {}
  const ExpressionLayout * const * array() { return const_cast<const ExpressionLayout * const *>(m_data); }
private:
  const ExpressionLayout * m_data[3];
};

}

#endif
