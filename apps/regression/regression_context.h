#ifndef REGRESSION_REGRESSION_CONTEXT_H
#define REGRESSION_REGRESSION_CONTEXT_H

#include <poincare/context.h>
#include <poincare/symbol.h>
#include "store.h"
#include "../shared/store_context.h"

namespace Regression {

class RegressionContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;
  // This is still very dirty but will be fixed in later commits, when Lists will be implemented
  static bool IsSymbol(const char * c, Poincare::Context * ctx = nullptr) { return (c[2] == 0 && (c[0] == Regression::Store::k_columnNames[0][0] || c[0] == Regression::Store::k_columnNames[1][0]) && c[1] >= '1' && c[1] <= '3'); }
};

}

#endif
