#include "local_context.h"

using namespace Poincare;

namespace Sequence {

LocalContext::LocalContext(Context * parentContext) :
  VariableContext('n', parentContext),
  m_unValue(Complex::Float(NAN)),
  m_un1Value(Complex::Float(NAN)),
  m_vnValue(Complex::Float(NAN)),
  m_vn1Value(Complex::Float(NAN)),
  m_wnValue(Complex::Float(NAN)),
  m_wn1Value(Complex::Float(NAN))
{
}

const Expression * LocalContext::expressionForSymbol(const Symbol * symbol) {
  switch (symbol->name()) {
    case Symbol::SpecialSymbols::un:
      return &m_unValue;
    case Symbol::SpecialSymbols::un1:
      return &m_un1Value;
    case Symbol::SpecialSymbols::vn:
      return &m_vnValue;
    case Symbol::SpecialSymbols::vn1:
      return &m_vn1Value;
    case Symbol::SpecialSymbols::wn:
      return &m_wnValue;
    case Symbol::SpecialSymbols::wn1:
      return &m_wn1Value;
    default:
      return VariableContext::expressionForSymbol(symbol);
  }
}

void LocalContext::setValueForSequenceRank(float value, const char * sequenceName, int rank) {
  switch (rank) {
    case 0:
      if (sequenceName[0] == 'u') {
        m_unValue = Complex::Float(value);
      }
      if (sequenceName[0] == 'v') {
        m_vnValue = Complex::Float(value);
      }
      if (sequenceName[0] == 'w') {
        m_wnValue = Complex::Float(value);
      }
      return;
    case 1:
      if (sequenceName[0] == 'u') {
        m_un1Value = Complex::Float(value);
      }
      if (sequenceName[0] == 'v') {
        m_vn1Value = Complex::Float(value);
      }
      if (sequenceName[0] == 'v') {
        m_wn1Value = Complex::Float(value);
      }
      return;
    default:
      return;
  }
}

}
