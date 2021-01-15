#include "sequence_cache_context.h"
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
SequenceCacheContext<T>::SequenceCacheContext(SequenceContext * sequenceContext) :
  ContextWithParent(sequenceContext),
  m_values{{NAN, NAN},{NAN, NAN},{NAN,NAN}},
  m_sequenceContext(sequenceContext)
{
}

template<typename T>
const Expression SequenceCacheContext<T>::expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
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
    if (!record.isNull()) {
      Sequence * seq = m_sequenceContext->sequenceStore()->modelForRecord(record);
      rank.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Float<T>::Builder(unknownSymbolValue));
      T n = PoincareHelpers::ApproximateToScalar<T>(rank, this);
      // In case the sequence referenced is not defined or if the rank is not an int, return NAN
      if (seq->fullName() != nullptr) {
        if (std::floor(n) == n) {
          Expression sequenceExpression = seq->expressionReduced(this);
          if (seq->hasValidExpression(this)) {
            return Float<T>::Builder(seq->valueAtRank<T>(n, m_sequenceContext));
          }
        }
      }
    } else {
      return Float<T>::Builder(NAN);
    }
  }
  return ContextWithParent::expressionForSymbolAbstract(symbol, clone);
}

template<typename T>
void SequenceCacheContext<T>::setValueForSymbol(T value, const Poincare::Symbol & symbol) {
  m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)] = value;
}

template<typename T>
int SequenceCacheContext<T>::nameIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(symbol.name()[0] >= 'u' && symbol.name()[0] <= 'w'); //  [u|v|w]
  char name = symbol.name()[0];
  assert(name >= SequenceStore::k_sequenceNames[0][0] && name <= SequenceStore::k_sequenceNames[MaxNumberOfSequences-1][0]); // u, v or w
  return name - 'u';
}

template<typename T>
int SequenceCacheContext<T>::rankIndexForSymbol(const Poincare::Symbol & symbol) {
  assert(strcmp(symbol.name()+1, "(n)") == 0 || strcmp(symbol.name()+1, "(n+1)") == 0); // u(n) or u(n+1)
  if (symbol.name()[3] == ')') { // (n)
    return 0;
  }
  // (n+1)
  return 1;
}

template class SequenceCacheContext<float>;
template class SequenceCacheContext<double>;

}
