#include "cache_context.h"
#include <cmath>

using namespace Poincare;

namespace Sequence {

template<typename T>
CacheContext<T>::CacheContext(Context * parentContext) :
  VariableContext<T>('n', parentContext),
  m_values{{Complex<T>::Float(NAN), Complex<T>::Float(NAN)},
    {Complex<T>::Float(NAN), Complex<T>::Float(NAN)}}
{
}

template<typename T>
const Expression * CacheContext<T>::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::un || symbol->name() == Symbol::SpecialSymbols::un1 ||
      symbol->name() == Symbol::SpecialSymbols::vn || symbol->name() == Symbol::SpecialSymbols::vn1) {
    return &m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)];
  }
  return VariableContext<T>::expressionForSymbol(symbol);
}

template<typename T>
void CacheContext<T>::setValueForSymbol(T value, const Poincare::Symbol * symbol) {
  m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = Complex<T>::Float(value);
}

template<typename T>
int CacheContext<T>::nameIndexForSymbol(const Poincare::Symbol * symbol) {
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

template<typename T>
int CacheContext<T>::rankIndexForSymbol(const Poincare::Symbol * symbol) {
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

template class CacheContext<float>;
template class CacheContext<double>;

}
