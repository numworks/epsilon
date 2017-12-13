#include <poincare/symbol.h>
#include <poincare/context.h>
#include <poincare/complex.h>
#include "layout/baseline_relative_layout.h"
#include "layout/string_layout.h"
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

const char * Symbol::textForSpecialSymbols(char name) const {
  switch (name) {
    case SpecialSymbols::Ans:
      return "ans";
    case SpecialSymbols::un:
      return "u(n)";
    case SpecialSymbols::un1:
      return "u(n+1)";
    case SpecialSymbols::vn:
      return "v(n)";
    case SpecialSymbols::vn1:
      return "v(n+1)";
    case SpecialSymbols::M0:
      return "M0";
    case SpecialSymbols::M1:
      return "M1";
    case SpecialSymbols::M2:
      return "M2";
    case SpecialSymbols::M3:
      return "M3";
    case SpecialSymbols::M4:
      return "M4";
    case SpecialSymbols::M5:
      return "M5";
    case SpecialSymbols::M6:
      return "M6";
    case SpecialSymbols::M7:
      return "M7";
    case SpecialSymbols::M8:
      return "M8";
    case SpecialSymbols::M9:
      return "M9";
    default:
      assert(false);
      return nullptr;
  }
}


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

Symbol::Symbol(Symbol&& other) :
  m_name(other.m_name)
{
}

Symbol::Symbol(const Symbol& other) :
  m_name(other.m_name)
{
}

Expression * Symbol::clone() const {
  return new Symbol(m_name);
}

Expression::Sign Symbol::sign() const {
  /* TODO: Maybe, we will want to know that from a context given in parameter:
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->sign(context);
  }*/
  if (m_name == Ion::Charset::SmallPi) {
    return Sign::Positive;
  }
  if (m_name == Ion::Charset::Exponential) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

template<typename T>
Expression * Symbol::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->approximate<T>(context, angleUnit);
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
  if (isMatrixSymbol()) {
    const char mi[] = { 'M', (char)(m_name-(char)SpecialSymbols::M0+'0') };
    return new StringLayout(mi, sizeof(mi));
  }
  return new StringLayout(&m_name, 1);
}

int Symbol::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  if (bufferSize == 1) {
    buffer[bufferSize-1] = 0;
    return 0;
  }
  /* Special cases for all special symbols */
  if (m_name >0 && m_name < 32) {
    return strlcpy(buffer, textForSpecialSymbols(m_name), bufferSize);
  }
  buffer[0] = m_name;
  buffer[1] = 0;
  return 1;
}

bool Symbol::isMatrixSymbol() const {
  if (m_name >= (char)SpecialSymbols::M0 && m_name <= (char)SpecialSymbols::M9) {
    return true;
  }
  return false;
}

bool Symbol::isScalarSymbol() const {
  if (m_name >= 'A' && m_name <= 'Z') {
    return true;
  }
  return false;
}

int Symbol::simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const {
  assert(e->type() == Expression::Type::Symbol);
  if ((uint8_t)m_name == ((uint8_t)static_cast<const Symbol *>(e)->name())) {
    return 0;
  }
  if ((uint8_t)m_name > ((uint8_t)static_cast<const Symbol *>(e)->name())) {
    return 1;
  }
  return -1;
}

}
