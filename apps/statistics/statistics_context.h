#ifndef STATISTICS_STATISTICS_CONTEXT_H
#define STATISTICS_STATISTICS_CONTEXT_H

#include "../shared/store_context.h"

namespace Statistics {

class StatisticsContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override;
};

}

#endif
