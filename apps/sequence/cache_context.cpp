#include "cache_context.h"
#include <cmath>

using namespace Poincare;

namespace Sequence {

CacheContext::CacheContext(Context * parentContext) :
  VariableContext('n', parentContext),
  m_values{{new Undefined(), new Undefined()},
    {new Undefined(), new Undefined()}}
{
}

CacheContext::~CacheContext() {
  Poincare::Expression * m_values[MaxNumberOfSequences][MaxRecurrenceDepth];
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    for (int j = 0; j < MaxRecurrenceDepth; j++) {
      if (m_values[i][j]) {
        delete m_values[i][j];
      }
    }
  }
}

const Expression * CacheContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::un || symbol->name() == Symbol::SpecialSymbols::un1 ||
      symbol->name() == Symbol::SpecialSymbols::vn || symbol->name() == Symbol::SpecialSymbols::vn1) {
    return m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)];
  }
  return VariableContext::expressionForSymbol(symbol);
}

template<typename T>
void CacheContext::setValueForSymbol(T value, const Poincare::Symbol * symbol) {
  if (m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)]) {
    delete m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)];
    m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = nullptr;
  }
  m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = Expression::CreateDecimal(value);
}

int CacheContext::nameIndexForSymbol(const Poincare::Symbol * symbol) {
  switch (symbol->name()) {
    case Symbol::SpecialSymbols::un:
      return 0;
    case Symbol::SpecialSymbols::un1:
      return 0;
    case Symbol::SpecialSymbols::vn:
      return 1;
    case Symbol::SpecialSymbols::vn1:
      return 1;
    default:
      return 0;
  }
}

int CacheContext::rankIndexForSymbol(const Poincare::Symbol * symbol) {
  switch (symbol->name()) {
    case Symbol::SpecialSymbols::un:
      return 0;
    case Symbol::SpecialSymbols::un1:
      return 1;
    case Symbol::SpecialSymbols::vn:
      return 0;
    case Symbol::SpecialSymbols::vn1:
      return 1;
    default:
      return 0;
  }
}

template void CacheContext::setValueForSymbol<double>(double, Poincare::Symbol const*);
template void CacheContext::setValueForSymbol<float>(float, Poincare::Symbol const*);

}
