#ifndef SEQUENCE_LOCAL_CONTEXT_H
#define SEQUENCE_LOCAL_CONTEXT_H

#include <poincare.h>

namespace Sequence {

class LocalContext : public Poincare::VariableContext {
public:
  LocalContext(Poincare::Context * parentContext);
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
  void setSequenceRankValue(float f, int rank);
private:
  char sequenceIndexForSymbol(const Poincare::Symbol * symbol);
  Poincare::Complex m_nValue;
  Poincare::Complex m_n1Value;
};

}

#endif
