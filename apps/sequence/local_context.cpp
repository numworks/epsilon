#include "local_context.h"

using namespace Poincare;

namespace Sequence {

LocalContext::LocalContext(Context * parentContext) :
  VariableContext('n', parentContext),
  m_values{{Complex::Float(NAN), Complex::Float(NAN)}, {Complex::Float(NAN), Complex::Float(NAN)}
  //, {Complex::Float(NAN), Complex::Float(NAN)}
  }
{
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::un || symbol->name() == Symbol::SpecialSymbols::un1 ||
      symbol->name() == Symbol::SpecialSymbols::vn || symbol->name() == Symbol::SpecialSymbols::vn1 ||
symbol->name() == Symbol::SpecialSymbols::wn || symbol->name() == Symbol::SpecialSymbols::wn1) {
    return &m_values[nameIndexForSymbol(symbol)][rankIndexForSymbol(symbol)];
  }
  return VariableContext::expressionForSymbol(symbol);
}

void LocalContext::setValueForSequenceRank(float value, const char * sequenceName, int rank) {
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    if (strcmp(sequenceName, SequenceStore::k_sequenceNames[i]) == 0) {
      m_values[i][rank] = Complex::Float(value);
    }
  }
}

int LocalContext::nameIndexForSymbol(const Poincare::Symbol * symbol) {
  switch (symbol->name()) {
    case Symbol::SpecialSymbols::un:
      return 0;
    case Symbol::SpecialSymbols::un1:
      return 0;
    case Symbol::SpecialSymbols::vn:
      return 1;
    case Symbol::SpecialSymbols::vn1:
      return 1;
    case Symbol::SpecialSymbols::wn:
      return 2;
    case Symbol::SpecialSymbols::wn1:
      return 2;
    default:
      return 0;
  }
}

int LocalContext::rankIndexForSymbol(const Poincare::Symbol * symbol) {
  switch (symbol->name()) {
    case Symbol::SpecialSymbols::un:
      return 0;
    case Symbol::SpecialSymbols::un1:
      return 1;
    case Symbol::SpecialSymbols::vn:
      return 0;
    case Symbol::SpecialSymbols::vn1:
      return 1;
    case Symbol::SpecialSymbols::wn:
      return 0;
    case Symbol::SpecialSymbols::wn1:
      return 1;
    default:
      return 0;
  }
}

}
