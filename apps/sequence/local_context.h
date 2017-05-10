#ifndef SEQUENCE_LOCAL_CONTEXT_H
#define SEQUENCE_LOCAL_CONTEXT_H

#include <poincare.h>

namespace Sequence {

class LocalContext : public Poincare::VariableContext {
public:
  LocalContext(Poincare::Context * parentContext);
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
  void setValueForSequenceRank(float value, const char * sequenceName, int rank);
private:
  Poincare::Complex m_unValue;
  Poincare::Complex m_un1Value;
  Poincare::Complex m_vnValue;
  Poincare::Complex m_vn1Value;
  Poincare::Complex m_wnValue;
  Poincare::Complex m_wn1Value;
};

}

#endif
