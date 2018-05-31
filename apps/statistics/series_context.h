#ifndef STATISTICS_SERIES_CONTEXT_H
#define STATISTICS_SERIES_CONTEXT_H

#include <poincare/context.h>
#include "../shared/float_pair_store.h"

namespace Statistics {

class SeriesContext : public Poincare::Context {
public:
  SeriesContext(Shared::FloatPairStore * store) :
    Poincare::Context(),
    m_store(store),
    m_seriesPairIndex(-1)
  {}
  void setStorePosition(int series, int i, int j);
  void setExpressionForSymbolName(const Poincare::Expression * expression, const Poincare::Symbol * symbol, Poincare::Context & context) override {}
  const Poincare::Expression * expressionForSymbol(const Poincare::Symbol * symbol) override;
private:
  Shared::FloatPairStore * m_store;
  int m_seriesPairIndex;
};

}

#endif
