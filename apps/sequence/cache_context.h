#ifndef SEQUENCE_CACHE_CONTEXT_H
#define SEQUENCE_CACHE_CONTEXT_H

#include <poincare.h>
#include "sequence_context.h"

namespace Sequence {

template<typename T>
class CacheContext : public Poincare::VariableContext<T> {
public:
  CacheContext(Poincare::Context * parentContext);
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
  void setValueForSymbol(T value, const Poincare::Symbol * symbol);
private:
  int nameIndexForSymbol(const Poincare::Symbol * symbol);
  int rankIndexForSymbol(const Poincare::Symbol * symbol);
  Poincare::Complex<T> m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
};

}

#endif
