#include "local_context.h"

using namespace Poincare;

namespace Sequence {

LocalContext::LocalContext(Context * parentContext) :
  VariableContext('n', parentContext),
  m_nValue(Complex::Float(NAN)),
  m_n1Value(Complex::Float(NAN))
{
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  if (sequenceIndexForSymbol(symbol) == 0) {
    return &m_nValue;
  }
  if (sequenceIndexForSymbol(symbol) == 1) {
    return &m_n1Value;
  }
  return VariableContext::expressionForSymbol(symbol);
}

void LocalContext::setSequenceRankValue(float f, int rank) {
  if (rank == 0) {
    m_nValue = Complex::Float(f);
  }
  if (rank == 1) {
    m_n1Value = Complex::Float(f);
  }
}

char LocalContext::sequenceIndexForSymbol(const Symbol * symbol) {
  if (symbol->name() == Symbol::SpecialSymbols::un ||
  	  symbol->name() == Symbol::SpecialSymbols::vn ||
  	  symbol->name() == Symbol::SpecialSymbols::wn) {
  	return 0;
  }
  if (symbol->name() == Symbol::SpecialSymbols::un1 ||
  	  symbol->name() == Symbol::SpecialSymbols::vn1 ||
  	  symbol->name() == Symbol::SpecialSymbols::wn1) {
  	return 1;
  }
  return 2;
}

}
