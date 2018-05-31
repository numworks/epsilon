#include "series_context.h"
#include <poincare/decimal.h>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

const Expression * SeriesContext::expressionForSymbol(const Symbol * symbol) {
  assert(m_seriesPairIndex >= 0);
  if (!symbol->isSeriesSymbol()) {
    return nullptr;
  }
  const char * seriesName = Symbol::textForSpecialSymbols(symbol->name());
  assert(strlen(seriesName) == 2);

  int series = seriesName[1] - '0';
  assert(series >= 0 && series < Store::k_numberOfSeries);

  assert((seriesName[0] == 'V') || (seriesName[0] == 'N'));
  int storeI = seriesName[0] == 'V' ? 0 : 1;

  assert(m_seriesPairIndex < store->numberOfPairsOfSeries(series));

  Expression * result = new Decimal(m_store->get(series, storeI, m_seriesPairIndex));
  assert(result != nullptr);
  return result;
}

}
