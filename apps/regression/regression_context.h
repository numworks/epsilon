#ifndef REGRESSION_REGRESSION_CONTEXT_H
#define REGRESSION_REGRESSION_CONTEXT_H

#include <poincare/context.h>
#include "../shared/store_context.h"

namespace Regression {

class RegressionContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;
  const Poincare::Expression expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) override;
};

}

#endif
