#include "regression_context.h"
#include <assert.h>
#include <string.h>

#include <poincare/float.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

const Expression RegressionContext::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  if (symbol.type() == ExpressionNode::Type::Symbol && Symbol::isRegressionSymbol(symbol.name(), nullptr)) {
    const char * seriesName = symbol.name();
    assert(strlen(seriesName) == 2);

    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);

    assert((seriesName[0] == 'X') || (seriesName[0] == 'Y'));
    int storeI = seriesName[0] == 'X' ? 0 : 1;

    assert(m_seriesPairIndex >= 0);
    assert(m_seriesPairIndex < m_store->numberOfPairsOfSeries(series));
    return Float<double>::Builder(m_store->get(series, storeI, m_seriesPairIndex));
  } else {
    return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
  }
}

}
