#ifndef STATISTICS_STATISTICS_CONTEXT_H
#define STATISTICS_STATISTICS_CONTEXT_H

#include <poincare/symbol.h>
#include <apps/shared/store_context.h>
#include "../store.h"

namespace Statistics {

class StatisticsContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;
  // This is still very dirty but will be fixed in later commits, when Lists will be implemented
  static bool IsSymbol(const char * c, Poincare::Context * ctx = nullptr) { return (c[2] == 0 && (c[0] == Statistics::Store::k_columnNames[0][0] || c[0] == Statistics::Store::k_columnNames[1][0]) && c[1] >= '1' && c[1] <= '3'); }
};

}

#endif
