#ifndef STATISTICS_STATISTICS_CONTEXT_H
#define STATISTICS_STATISTICS_CONTEXT_H

#include <poincare/symbol.h>
#include <apps/shared/store_context.h>

namespace Statistics {

class StatisticsContext : public Shared::StoreContext {
public:
  using Shared::StoreContext::StoreContext;

private:
  bool isSymbol(const char * name) const override { return Poincare::Symbol::isSeriesSymbol(name, nullptr); }
  int getSymbolColumn(const char * name) const override { return name[0] == 'N'; }
};

}

#endif
