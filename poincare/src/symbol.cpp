#include <poincare/symbol.h>
#include <poincare/context.h>
#include <poincare/rational.h>
#include <poincare/parenthesis.h>
#include <poincare/layout_helper.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

ExpressionNode::Sign SymbolNode::sign() const {
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

Expression SymbolNode::replaceSymbolWithExpression(char symbol, Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
}

int SymbolNode::polynomialDegree(char symbol) const {
  if (m_name == symbol) {
    return 1;
  }
  return 0;
}

int SymbolNode::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  return Symbol(this).getPolynomialCoefficients(symbolName, coefficients);
}

int SymbolNode::getVariables(isVariableTest isVariable, char * variables) const {
 size_t variablesLength = strlen(variables);
 if (isVariable(m_name)) {
   char * currentChar = variables;
   while (*currentChar != 0) {
     if (*currentChar == m_name) {
       return variablesLength;
     }
     currentChar++;
   }
   if (variablesLength < Expression::k_maxNumberOfVariables) {
     variables[variablesLength] = m_name;
     variables[variablesLength+1] = 0;
     return variablesLength+1;
   }
   return -1;
 }
 return variablesLength;
}

float SymbolNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  if (m_name == 'x') {
    return NAN;
  }
  return 0.0;
}

int SymbolNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Symbol);
  if ((uint8_t)m_name == ((uint8_t)static_cast<const SymbolNode *>(e)->name())) {
    return 0;
  }
  if ((uint8_t)m_name > ((uint8_t)static_cast<const SymbolNode *>(e)->name())) {
    return 1;
  }
  return -1;
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_name == Symbol::SpecialSymbols::Ans) {
    return LayoutHelper::String("ans", 3);
  }
  if (m_name == Symbol::SpecialSymbols::un) {
    return HorizontalLayout(
        CharLayout('u'),
        VerticalOffsetLayout(
          CharLayout('n'),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (m_name == Symbol::SpecialSymbols::un1) {
    return HorizontalLayout(
      CharLayout('u'),
      VerticalOffsetLayout(
        LayoutHelper::String("n+1", 3),
        VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (m_name == Symbol::SpecialSymbols::vn) {
    return HorizontalLayout(
        CharLayout('v'),
        VerticalOffsetLayout(
          CharLayout('n'),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (m_name == Symbol::SpecialSymbols::vn1) {
    return HorizontalLayout(
      CharLayout('v'),
      VerticalOffsetLayout(
        LayoutHelper::String("n+1", 3),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (Symbol::isMatrixSymbol(m_name) || Symbol::isSeriesSymbol(m_name) || Symbol::isRegressionSymbol(m_name)) {
    return LayoutHelper::String(Symbol::textForSpecialSymbols(m_name), 2);
  }
  return LayoutHelper::String(&m_name, 1);
}

int SymbolNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  if (bufferSize == 1) {
    buffer[0] = 0;
    return 0;
  }
  /* Special cases for all special symbols */
  if (m_name >0 && m_name < 32) {
    return strlcpy(buffer, Symbol::textForSpecialSymbols(m_name), bufferSize);
  }
  buffer[0] = m_name;
  buffer[1] = 0;
  return 1;
}

Expression SymbolNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Symbol(this).shallowReduce(context, angleUnit);
}

bool SymbolNode::hasAnExactRepresentation(Context & context) const {
  // TODO: so far, no symbols can be exact but A, ..Z should be able to hold exact values later.
  return false;
}

template<typename T>
Evaluation<T> SymbolNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  if (m_name == Ion::Charset::IComplex) {
    return Complex<T>(0.0, 1.0);
  }
  const Expression e = context.expressionForSymbol(Symbol(m_name));
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.approximateToEvaluation<T>(context, angleUnit);
}

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

bool Symbol::isMatrixSymbol(char c) {
  if (c >= (char)SpecialSymbols::M0 && c <= (char)SpecialSymbols::M9) {
    return true;
  }
  return false;
}

bool Symbol::isScalarSymbol(char c) {
  if (c >= 'A' && c <= 'Z') {
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

bool Symbol::isApproximate(char c, Context & context) {
  // TODO: so far, all symbols A to Z, M0->M9 hold an approximate values. But they should be able to hold exact values later.
  if (Symbol::isScalarSymbol(c) || Symbol::isMatrixSymbol(c)) {
    return true;
  }
  return false;
}

Expression Symbol::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  // Do not replace symbols in expression of type: 3->A
  Expression p = parent();
  if (!p.isUninitialized() && p.type() == ExpressionNode::Type::Store && p.childAtIndex(1) == *this) {
    return *this;
  }
  const Expression e = context.expressionForSymbol(*this);
  if (!e.isUninitialized() && node()->hasAnExactRepresentation(context)) {
    // TODO: later AZ should be replaced.
    // The stored expression had been beautified, so we need to call deepReduce
    Expression result = e.clone();
    replaceWithInPlace(result);
    return result.deepReduce(context, angleUnit);
  }
  return *this;
}

Expression Symbol::replaceSymbolWithExpression(char symbol, Expression & expression) {
  if (name() == symbol) {
    Expression value = expression.clone();
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childNeedsParenthesis(value.node())) {
      value = Parenthesis(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

int Symbol::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  if (name() == symbolName) {
    coefficients[0] = Rational(0);
    coefficients[1] = Rational(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

}
