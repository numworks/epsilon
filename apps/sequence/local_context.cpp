#include "local_context.h"
#include <cmath>

using namespace Poincare;

namespace Sequence {

template<typename T>
LocalContext<T>::LocalContext(Context * parentContext) :
  VariableContext<T>('n', parentContext),
  m_values{{Complex<T>::Float(NAN), Complex<T>::Float(NAN)},
    {Complex<T>::Float(NAN), Complex<T>::Float(NAN)}}
{
}

template<typename T>
const Expression * LocalContext<T>::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::un || symbol->name() == Symbol::SpecialSymbols::un1 ||
      symbol->name() == Symbol::SpecialSymbols::vn || symbol->name() == Symbol::SpecialSymbols::vn1) {
    return &m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)];
  }
  return VariableContext<T>::expressionForSymbol(symbol);
}

template<typename T>
void LocalContext<T>::setValueForSequenceRank(T value, const char * sequenceName, int rank) {
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    if (strcmp(sequenceName, SequenceStore::k_sequenceNames[i]) == 0) {
      m_values[i][rank] = Complex<T>::Float(value);
    }
  }
}

template<typename T>
int LocalContext<T>::nameIndexForSymbol(const Poincare::Symbol * symbol) {
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
int LocalContext<T>::rankIndexForSymbol(const Poincare::Symbol * symbol) {
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

template class LocalContext<float>;
template class LocalContext<double>;

}
