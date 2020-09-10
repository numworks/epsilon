#include "cache_context.h"
#include "sequence.h"
#include "sequence_store.h"
#include "poincare_helpers.h"
#include <poincare/serialization_helper.h>
#include <poincare/addition.h>
#include <poincare/rational.h>
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
const Expression CacheContext<T>::expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
  // [u|v|w](n(+1)?)
  if (symbol.type() == ExpressionNode::Type::Sequence) {
    int index = nameIndexForSymbol(const_cast<Symbol &>(static_cast<const Symbol &>(symbol)));
    Expression rank = symbol.childAtIndex(0).clone();
    if (rank.isIdenticalTo(Symbol::Builder(UCodePointUnknown))) {
      return Float<T>::Builder(m_values[index][0]);
    } if (rank.isIdenticalTo(Addition::Builder(Symbol::Builder(UCodePointUnknown), Rational::Builder(1)))) {
      return Float<T>::Builder(m_values[index][1]);
    }
    Ion::Storage::Record record = m_sequenceContext->sequenceStore()->recordAtIndex(index);
    Sequence * seq = m_sequenceContext->sequenceStore()->modelForRecord(record);
    rank.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Float<T>::Builder(m_nValue));
    T n = PoincareHelpers::ApproximateToScalar<T>(rank, this);
    // In case the rank is not int or sequence referenced is not defined, return NAN
    if (std::floor(n) == n && seq->fullName() != nullptr) {
      return Float<T>::Builder(seq->valueAtRank<T>(n, m_sequenceContext));
    } else {
      return Float<T>::Builder(NAN);
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
