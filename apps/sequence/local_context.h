#ifndef SEQUENCE_LOCAL_CONTEXT_H
#define SEQUENCE_LOCAL_CONTEXT_H

#include <poincare.h>
#include "sequence_store.h"

namespace Sequence {

class LocalContext : public Poincare::VariableContext {
public:
  LocalContext(Poincare::Context * parentContext);
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
  void setValueForSequenceRank(float value, const char * sequenceName, int rank);
private:
  constexpr static int k_depth = 2;
  int nameIndexForSymbol(const Poincare::Symbol * symbol);
  int rankIndexForSymbol(const Poincare::Symbol * symbol);
  Poincare::Complex m_values[SequenceStore::k_maxNumberOfSequences][k_depth];
};

}

#endif
