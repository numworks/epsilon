#include "cache_context.h"
#include "sequence.h"
#include "sequence_store.h"
#include "../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>
#include <cmath>

using namespace Poincare;

namespace Sequence {

template<typename T>
CacheContext<T>::CacheContext(Context * parentContext) :
  ContextWithParent(parentContext),
  m_values{{NAN, NAN},{NAN, NAN},{NAN,NAN}}
{
}

template<typename T>
const Expression CacheContext<T>::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  // [u|v|w](n(+1)?)
  // u,v,w are reserved names. They can only be set through the sequence app
  if (symbol.type() == ExpressionNode::Type::Symbol
    && symbol.name()[0] >= SequenceStore::k_sequenceNames[0][0]
    && symbol.name()[0] <=  SequenceStore::k_sequenceNames[MaxNumberOfSequences-1][0]) {
    assert((symbol.name()+1)[0] == '(');
    Symbol s = const_cast<Symbol &>(static_cast<const Symbol &>(symbol));
    if (strcmp(symbol.name()+1, "(n)") == 0 || strcmp(symbol.name()+1, "(n+1)") == 0) {
      return Float<T>::Builder(m_values[nameIndexForSymbol(s)][rankIndexForSymbol(s)]);
    } else {
      Sequence seq = m_sequenceContext->sequenceStore()->sequenceAtIndex(nameIndexForSymbol(s));
      // In case the sequence referenced is not defined, return NAN
      if (seq.fullName() == nullptr) {
        return Float<T>::Builder(NAN);
      }
      int numberOfDigits = 1;
      constexpr int offset = 2; // 2 = 1 for ('u') + 1 for ('(')
      while (symbol.name()[offset+numberOfDigits] != ')') {
        numberOfDigits++;
      }
      // Get the value of k in u(k) and store it in x
      Integer integer(symbol.name()+2, numberOfDigits, false);
      T x = integer.approximate<T>();
      return Float<T>::Builder(seq.valueAtRank<T>(x, m_sequenceContext));
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
  assert(strlen(symbol.name()) >= 4); //  [u|v|w](n(+1) or k ?)
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
