#include <poincare/symbol.h>

#include <poincare/context.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>

#include <poincare/layout_engine.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/vertical_offset_layout.h"

#include <ion.h>
#include <cmath>

extern "C" {
#include <assert.h>
}

namespace Poincare {

const char * Symbol::textForSpecialSymbols(char name) {
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
    case SpecialSymbols::V1:
      return "V1";
    case SpecialSymbols::N1:
      return "N1";
    case SpecialSymbols::V2:
      return "V2";
    case SpecialSymbols::N2:
      return "N2";
    case SpecialSymbols::V3:
      return "V3";
    case SpecialSymbols::N3:
      return "N3";
    case SpecialSymbols::X1:
      return "X1";
    case SpecialSymbols::Y1:
      return "Y1";
    case SpecialSymbols::X2:
      return "X2";
    case SpecialSymbols::Y2:
      return "Y2";
    case SpecialSymbols::X3:
      return "X3";
    case SpecialSymbols::Y3:
      return "Y3";
    default:
      assert(false);
      return nullptr;
  }
}

int Symbol::getVariables(isVariableTest isVariable, char * variables) const {
 size_t variablesLength = strlen(variables);
 if (isVariable(m_name)) {
   char * currentChar = variables;
   while (*currentChar != 0) {
     if (*currentChar == m_name) {
       return variablesLength;
     }
     currentChar++;
   }
   if (variablesLength < k_maxNumberOfVariables) {
     variables[variablesLength] = m_name;
     variables[variablesLength+1] = 0;
     return variablesLength+1;
   }
   return -1;
 }
 return variablesLength;
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

int Symbol::polynomialDegree(char symbol) const {
  if (m_name == symbol) {
    return 1;
  }
  return 0;
}

int Symbol::privateGetPolynomialCoefficients(char symbolName, Expression * coefficients[]) const {
  if (m_name == symbolName) {
    coefficients[0] = new Rational(0);
    coefficients[1] = new Rational(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

Expression * Symbol::replaceSymbolWithExpression(char symbol, Expression * expression) {
  if (m_name == symbol) {
    Expression * value = expression->clone();
    if (parent() && value->needParenthesisWithParent(parent())) {
      value = new Parenthesis(value, false);
    }
    return replaceWith(value, true);
  }
  return this;
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

bool Symbol::isApproximate(Context & context) const {
  // TODO: so far, all symbols A to Z, M0->M9 hold an approximate values. But they should be able to hold exact values later.
  if (isScalarSymbol() || isMatrixSymbol()) {
    return true;
  }
  return false;
}

float Symbol::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  if (m_name == 'x') {
    return NAN;
  }
  return 0.0;
}

bool Symbol::hasAnExactRepresentation(Context & context) const {
  // TODO: so far, no symbols can be exact but A, ..Z should be able to hold exact values later.
  return false;
}

Expression * Symbol::shallowReduce(Context& context, AngleUnit angleUnit) {
  // Do not replace symbols in expression of type: 3->A
  if (parent()->type() == Type::Store && parent()->operand(1) == this) {
    return this;
  }
  const Expression * e = context.expressionForSymbol(this);
  if (e != nullptr && hasAnExactRepresentation(context)) { // TODO: later A...Z should be replaced.
    /* The stored expression had been beautified which forces to call deepReduce. */
    return replaceWith(e->clone(), true)->deepReduce(context, angleUnit);
  }
  return this;
}

template<typename T>
Evaluation<T> * Symbol::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  if (m_name == Ion::Charset::IComplex) {
    return new Complex<T>(0.0, 1.0);
  }
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->privateApproximate(T(), context, angleUnit);
  }
  return new Complex<T>(Complex<T>::Undefined());
}

Expression::Type Symbol::type() const {
  return Expression::Type::Symbol;
}

char Symbol::name() const {
  return m_name;
}

ExpressionLayout * Symbol::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_name == SpecialSymbols::Ans) {
    return LayoutEngine::createStringLayout("ans", 3);
  }
  if (m_name == SpecialSymbols::un) {
    return new HorizontalLayout(
        new CharLayout('u'),
        new VerticalOffsetLayout(
          new CharLayout('n'),
          VerticalOffsetLayout::Type::Subscript,
          false),
        false);
  }
  if (m_name == SpecialSymbols::un1) {
    return new HorizontalLayout(
      new CharLayout('u'),
      new VerticalOffsetLayout(
        LayoutEngine::createStringLayout("n+1", 3),
        VerticalOffsetLayout::Type::Subscript,
        false),
      false);
  }
  if (m_name == SpecialSymbols::vn) {
    return new HorizontalLayout(
        new CharLayout('v'),
        new VerticalOffsetLayout(
          new CharLayout('n'),
          VerticalOffsetLayout::Type::Subscript,
          false),
        false);
  }
  if (m_name == SpecialSymbols::vn1) {
    return new HorizontalLayout(
      new CharLayout('v'),
      new VerticalOffsetLayout(
        LayoutEngine::createStringLayout("n+1", 3),
        VerticalOffsetLayout::Type::Subscript,
        false),
      false);
  }
  if (isMatrixSymbol() || isSeriesSymbol(m_name) || isRegressionSymbol(m_name)) {
    return LayoutEngine::createStringLayout(textForSpecialSymbols(m_name), 2);
  }
  return LayoutEngine::createStringLayout(&m_name, 1);
}

int Symbol::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
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

bool Symbol::isVariableSymbol(char c)  {
  if (c >= 'a' && c <= 'z') {
    return true;
  }
  return false;
}

bool Symbol::isSeriesSymbol(char c) {
  if (c >= (char)SpecialSymbols::V1 && c <= (char)SpecialSymbols::N3) {
    return true;
  }
  return false;
}

bool Symbol::isRegressionSymbol(char c) {
  if (c >= (char)SpecialSymbols::X1 && c <= (char)SpecialSymbols::Y3) {
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
