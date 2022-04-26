#ifndef REGRESSION_REGRESSION_CONTEXT_H
#define REGRESSION_REGRESSION_CONTEXT_H

#include <poincare/context.h>
#include <poincare/symbol.h>
#include "../shared/store_context.h"

namespace Regression {

class RegressionContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;

private:
  bool isSymbol(const char * name) const override { return Poincare::Symbol::isRegressionSymbol(name, nullptr); }
  int getSymbolColumn(const char * name) const override { return name[0] == 'Y'; }
};

}

#endif
