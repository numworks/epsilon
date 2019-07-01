#include "cache_context.h"
#include "sequence_store.h"
#include <cmath>

using namespace Poincare;

namespace Sequence {

template<typename T>
CacheContext<T>::CacheContext(Context * parentContext) :
  m_values{{NAN, NAN},
    {NAN, NAN}},
  m_parentContext(parentContext)
{
}

template<typename T>
const Expression CacheContext<T>::expressionForSymbol(const SymbolAbstract & symbol, bool clone) {
  // [u|v](n(+1)?)
  if (symbol.type() == ExpressionNode::Type::Symbol
    && (symbol.name()[0] ==  SequenceStore::k_sequenceNames[0][0] || symbol.name()[0] ==  SequenceStore::k_sequenceNames[1][0])
    && (strcmp(symbol.name()+1, "(n)") == 0 || strcmp(symbol.name()+1, "(n+1)") == 0))
  {
    Symbol s = const_cast<Symbol &>(static_cast<const Symbol &>(symbol));
    return Float<T>::Builder(m_values[nameIndexForSymbol(s)][rankIndexForSymbol(s)]);
  }
  return m_parentContext->expressionForSymbol(symbol, clone);
}

template<typename T>
void CacheContext<T>::setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context * context) {
  m_parentContext->setExpressionForSymbol(expression, symbol, context);
}

template<typename T>
void CacheContext<T>::setValueForSymbol(T value, const Poincare::Symbol & symbol) {
  m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = value;
}

template<typename T>
int CacheContext<T>::nameIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(strlen(symbol.name()) == 4 || strlen(symbol.name()) == 6); //  [u|v](n(+1)?)
  if (symbol.name()[0] == SequenceStore::k_sequenceNames[0][0]) { // u
    return 0;
  }
  // v
  return 1;
}

template<typename T>
int CacheContext<T>::rankIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(strlen(symbol.name()) == 4 || strlen(symbol.name()) == 6); // u(n) or u(n+1)
  if (symbol.name()[3] == ')') { // .(n)
    return 0;
  }
  // .(n+1)
  return 1;
}

template class CacheContext<float>;
template class CacheContext<double>;

}
