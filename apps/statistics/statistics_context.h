#ifndef STATISTICS_STATISTICS_CONTEXT_H
#define STATISTICS_STATISTICS_CONTEXT_H

#include "../shared/store_context.h"

namespace Statistics {

class StatisticsContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
};

}

#endif
