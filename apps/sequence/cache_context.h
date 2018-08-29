#ifndef SEQUENCE_CACHE_CONTEXT_H
#define SEQUENCE_CACHE_CONTEXT_H

#include <poincare.h>
#include "sequence_context.h"

namespace Sequence {

class CacheContext : public Poincare::VariableContext {
public:
  CacheContext(Poincare::Context * parentContext);
  ~CacheContext();
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
  template<typename T> void setValueForSymbol(T value, const Poincare::Symbol * symbol);
private:
  int nameIndexForSymbol(const Poincare::Symbol * symbol);
  int rankIndexForSymbol(const Poincare::Symbol * symbol);
  Poincare::Expression * m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
};

}

#endif
