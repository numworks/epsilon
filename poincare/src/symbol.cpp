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

size_t SymbolNode::size() const {
  return sizeof(SymbolNode)+strlen(m_name)+1;
}

ExpressionNode::Sign SymbolNode::sign() const {
  /* TODO: Maybe, we will want to know that from a context given in parameter:
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->sign(context);
  }*/
  if (m_name[0] == Ion::Charset::SmallPi) {
    return Sign::Positive;
  }
  if (m_name[0] == Ion::Charset::Exponential) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

Expression SymbolNode::replaceSymbolWithExpression(char symbol, Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
}

int SymbolNode::polynomialDegree(Context & context, char symbolName) const {
  if (m_name == symbolName) {
    return 1;
  }
  return 0;
}

int SymbolNode::getPolynomialCoefficients(Context & context, char symbolName, Expression coefficients[]) const {
  return Symbol(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

int SymbolNode::getVariables(Context & context, isVariableTest isVariable, char * variables) const {
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
     variables[variablesLength] = m_name[0];
     variables[variablesLength+1] = 0;
     return variablesLength+1;
   }
   return -1;
 }
 return variablesLength;
}

float SymbolNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  if (m_name[0] == SpecialSymbols::UnknownX) {
    return NAN;
  }
  return 0.0;
}

int SymbolNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Symbol);
  return strlcmp(m_name, static_cast<const SymbolNode *>(e)->name());
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (strcmp(m_name, "u(n)") == 0) {
    return HorizontalLayout(
        CharLayout('u'),
        VerticalOffsetLayout(
          CharLayout('n'),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (strcmp(m_name, "u(n+1)") == 0) {
    return HorizontalLayout(
      CharLayout('u'),
      VerticalOffsetLayout(
        LayoutHelper::String("n+1", 3),
        VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (strcmp(m_name, "v(n)" ) == 0) {
    return HorizontalLayout(
        CharLayout('v'),
        VerticalOffsetLayout(
          CharLayout('n'),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  if (strcmp(m_name, "v(n+1)") == 0) {
    return HorizontalLayout(
      CharLayout('v'),
      VerticalOffsetLayout(
        LayoutHelper::String("n+1", 3),
          VerticalOffsetLayoutNode::Type::Subscript));
  }
  return LayoutHelper::String(m_name, strlen(m_name));
}

int SymbolNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return strlcpy(buffer, m_name, bufferSize);
}

Expression SymbolNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Symbol(this).shallowReduce(context, angleUnit);
}

bool SymbolNode::shouldBeReplaceWhileReducing(Context & context) const {
  if (m_name[0] == Ion::Charset::SmallPi || m_name[0] == Ion::Charset::Exponential || Ion::Charset::IComplex) {
    return false;
  }
  return true;
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

bool Symbol::isVariableSymbol(char c)  {
  if (c >= 'a' && c <= 'z') {
    return true;
  }
  return false;
}

bool Symbol::isSeriesSymbol(char c) {
  // TODO
  return false;
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

bool Symbol::isApproximate(Context & context) {
  Expression e = context.expressionForSymbol(*this);
  return e.isApproximate(context);
}

Expression Symbol::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  // Do not replace symbols in expression of type: 3->A
  Expression p = parent();
  if (!p.isUninitialized() && p.type() == ExpressionNode::Type::Store && p.childAtIndex(1) == *this) {
    return *this;
  }
  const Expression e = context.expressionForSymbol(*this);
  if (!e.isUninitialized() && node()->shouldBeReplaceWhileReducing(context)) {
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

int Symbol::getPolynomialCoefficients(Context & context, char symbolName, Expression coefficients[]) const {
  if (name() == symbolName) {
    coefficients[0] = Rational(0);
    coefficients[1] = Rational(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

}
