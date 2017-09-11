#include <poincare/symbol.h>
#include <poincare/context.h>
#include <poincare/complex.h>
#include "layout/baseline_relative_layout.h"
#include "layout/string_layout.h"
#include <poincare/complex_matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Symbol::SpecialSymbols Symbol::matrixSymbol(char index) {
  switch (index - '0') {
    case 0:
      return SpecialSymbols::M0;
    case 1:
      return SpecialSymbols::M1;
    case 2:
      return SpecialSymbols::M2;
    case 3:
      return SpecialSymbols::M3;
    case 4:
      return SpecialSymbols::M4;
    case 5:
      return SpecialSymbols::M5;
    case 6:
      return SpecialSymbols::M6;
    case 7:
      return SpecialSymbols::M7;
    case 8:
      return SpecialSymbols::M8;
    case 9:
      return SpecialSymbols::M9;
    default:
      assert(false);
      return SpecialSymbols::M0;
  }
}

Symbol::Symbol(char name) :
  m_name(name)
{
}

template<typename T>
Evaluation<T> * Symbol::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->evaluate<T>(context, angleUnit);
  }
  return new Complex<T>(Complex<T>::Float(NAN));
}

Expression::Type Symbol::type() const {
  return Expression::Type::Symbol;
}

char Symbol::name() const {
  return m_name;
}

ExpressionLayout * Symbol::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  if (m_name == SpecialSymbols::Ans) {
    return new StringLayout("ans", 3);
  }
  if (m_name == SpecialSymbols::un) {
    return new BaselineRelativeLayout(new StringLayout("u", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::un1) {
    return new BaselineRelativeLayout(new StringLayout("u", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::vn) {
    return new BaselineRelativeLayout(new StringLayout("v", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::vn1) {
    return new BaselineRelativeLayout(new StringLayout("v", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::wn) {
    return new BaselineRelativeLayout(new StringLayout("w", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::wn1) {
    return new BaselineRelativeLayout(new StringLayout("w", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (isMatrixSymbol()) {
    const char mi[] = { 'M', (char)(m_name-(char)SpecialSymbols::M0+'0') };
    return new StringLayout(mi, sizeof(mi));
  }
  return new StringLayout(&m_name, 1);
}

Expression * Symbol::clone() const {
  return new Symbol(m_name);
}

bool Symbol::valueEquals(const Expression * e) const {
  assert(e->type() == Expression::Type::Symbol);
  return (m_name == ((Symbol *)e)->m_name);
}

bool Symbol::isMatrixSymbol() const {
  if (m_name >= (char)SpecialSymbols::M0 && m_name <= (char)SpecialSymbols::M9) {
    return true;
  }
  return false;
}

}
