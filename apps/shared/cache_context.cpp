#include "cache_context.h"
#include "sequence.h"
#include "sequence_store.h"
#include "../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

template<typename T>
CacheContext<T>::CacheContext(Context * parentContext) :
  ContextWithParent(parentContext),
  m_values{{NAN, NAN},{NAN, NAN},{NAN,NAN}}
{
}

template<typename T>
const Expression CacheContext<T>::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  // [u|v|w](n(+1)?)
  if (symbol.type() == ExpressionNode::Type::Sequence) {
    Symbol s = const_cast<Symbol &>(static_cast<const Symbol &>(symbol));
    if (s.childAtIndex(0).type() == ExpressionNode::Type::Symbol) {
      return Float<T>::Builder(m_values[nameIndexForSymbol(s)][0]);
    } else if (s.childAtIndex(0).type() == ExpressionNode::Type::Addition) {
      return Float<T>::Builder(m_values[nameIndexForSymbol(s)][1]);
    } else {
      Sequence seq = m_sequenceContext->sequenceStore()->sequenceAtIndex(nameIndexForSymbol(s));
      // In case the sequence referenced is not defined, return NAN
      if (seq.fullName() == nullptr) {
        return Float<T>::Builder(NAN);
      }
      Expression rank = symbol.childAtIndex(0);
      if (rank.isNumber()) {
        return Float<T>::Builder(seq.valueAtRank<T>(rank.approximateToScalar<double>(this, Poincare::Preferences::ComplexFormat::Cartesian, Poincare::Preferences::AngleUnit::Radian), m_sequenceContext));
      } else {
        return Float<T>::Builder(NAN);
      }
    }
  }
  return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
}

template<typename T>
void CacheContext<T>::setValueForSymbol(T value, const Poincare::Symbol & symbol) {
  m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = value;
}

template<typename T>
int CacheContext<T>::nameIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(symbol.name()[0] >= 'u' && symbol.name()[0] <= 'w'); //  [u|v|w]
  char name = symbol.name()[0];
  assert(name >= SequenceStore::k_sequenceNames[0][0] && name <= SequenceStore::k_sequenceNames[MaxNumberOfSequences-1][0]); // u, v or w
  return name - 'u';
}

template<typename T>
int CacheContext<T>::rankIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(strcmp(symbol.name()+1, "(n)") == 0 || strcmp(symbol.name()+1, "(n+1)") == 0); // u(n) or u(n+1)
  if (symbol.name()[3] == ')') { // (n)
    return 0;
  }
  // (n+1)
  return 1;
}

template class CacheContext<float>;
template class CacheContext<double>;

}
