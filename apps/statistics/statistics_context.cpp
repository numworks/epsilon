#include "statistics_context.h"
#include <assert.h>
#include <string.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

const Expression * StatisticsContext::expressionForSymbol(const Symbol * symbol) {
  if (Symbol::isSeriesSymbol(symbol->name())) {
    const char * seriesName = Symbol::textForSpecialSymbols(symbol->name());
    assert(strlen(seriesName) == 2);

    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);

    assert((seriesName[0] == 'V') || (seriesName[0] == 'N'));
    int storeI = seriesName[0] == 'V' ? 0 : 1;

    assert(m_seriesPairIndex >= 0);
    assert(m_seriesPairIndex < m_store->numberOfPairsOfSeries(series));
    m_value = Complex<double>::Float(m_store->get(series, storeI, m_seriesPairIndex));
    return &m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

}
