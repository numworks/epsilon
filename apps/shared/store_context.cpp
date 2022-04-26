#include "store_context.h"
#include <assert.h>
#include <string.h>
#include <poincare/float.h>

namespace Shared {

const Poincare::Expression StoreContext::expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
  if (symbol.type() == Poincare::ExpressionNode::Type::Symbol && isSymbol(symbol.name())) {
    const char * seriesName = symbol.name();
    assert(strlen(seriesName) == 2);

    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);

    int storeI = getSymbolColumn(seriesName);

    assert(m_seriesPairIndex >= 0);
    assert(m_seriesPairIndex < m_store->numberOfPairsOfSeries(series));
    return Poincare::Float<double>::Builder(m_store->get(series, storeI, m_seriesPairIndex));
  } else {
    return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
  }
}

Poincare::Context::SymbolAbstractType StoreContext::expressionTypeForIdentifier(const char * identifier, int length) {
  if (length == 2) {
    // Crop identifier
    char name[3] = {identifier[0], identifier[1], 0};
    if (isSymbol(name)) {
      return Context::SymbolAbstractType::Symbol;
    }
  }
  return Poincare::ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

}
