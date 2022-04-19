#include "store_context.h"
#include <assert.h>
#include <string.h>
#include <poincare/float.h>

namespace Shared {

const Poincare::Expression StoreContext::expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
  if (symbol.type() == Poincare::ExpressionNode::Type::Symbol) {
    const char * seriesName = symbol.name();
    int series;
    int column;
    if (m_store->isColumnName(seriesName, strlen(seriesName), &series, &column)) {
      assert(m_seriesPairIndex >= 0);
      assert(m_seriesPairIndex < m_store->numberOfPairsOfSeries(series));
      return Poincare::Float<double>::Builder(m_store->get(series, column, m_seriesPairIndex));
    }
  }
  return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
}

Poincare::Context::SymbolAbstractType StoreContext::expressionTypeForIdentifier(const char * identifier, int length) {
  if (m_store->isColumnName(identifier, length)) {
    return Context::SymbolAbstractType::Symbol;
  }
  return Poincare::ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

}
