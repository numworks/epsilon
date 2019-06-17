#include <poincare/symbol.h>
#include <poincare/code_point_layout.h>
#include <poincare/context.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parametered_expression.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <cmath>
#include <string.h>
#include <assert.h>

namespace Poincare {

constexpr char Symbol::k_ans[];

SymbolNode::SymbolNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

Expression SymbolNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
}

Expression SymbolNode::replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) {
  return Symbol(this).replaceUnknown(symbol, unknownSymbol);
}

int SymbolNode::polynomialDegree(Context & context, const char * symbolName) const {
  if (strcmp(m_name, symbolName) == 0) {
    return 1;
  }
  return 0;
}

int SymbolNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  return Symbol(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

int SymbolNode::getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
  // variables is in fact of type char[k_maxNumberOfVariables][maxSizeVariable]
  size_t variablesIndex = 0;
  while(variables[variablesIndex] != 0) {
    variablesIndex+= maxSizeVariable;
  }
  if (isVariable(m_name)) {
    int index = 0;
    while (variables[index] != 0) {
      if (strcmp(m_name, &variables[index]) == 0) {
        return variablesIndex/maxSizeVariable;
      }
      index+= maxSizeVariable;
    }
    if ((variablesIndex/maxSizeVariable) < Expression::k_maxNumberOfVariables) {
      if (strlen(m_name) + 1 > (size_t)maxSizeVariable) {
        return -2;
      }
      strlcpy(&variables[variablesIndex], m_name, maxSizeVariable);
      variables[variablesIndex+maxSizeVariable] = 0;
      return variablesIndex/maxSizeVariable+1;
    }
    return -1;
  }
  return variablesIndex/maxSizeVariable;
}

float SymbolNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  return isUnknown(UCodePointUnknownX) ? NAN : 0.0f;
}

bool SymbolNode::isReal(Context & context) const {
  Symbol s(this);
  return SymbolAbstract::isReal(s, context);
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (isUnknown(UCodePointUnknownX)) {
    return CodePointLayout::Builder('x');
  }
  if (isUnknown(UCodePointUnknownN)) {
    return CodePointLayout::Builder('n');
  }
  // TODO return Parse(m_name).createLayout() ?
  if (strcmp(m_name, "u(n)") == 0) {
    return HorizontalLayout::Builder(
        CodePointLayout::Builder('u'),
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayoutNode::Position::Subscript));
  }
  if (strcmp(m_name, "u(n+1)") == 0) {
    return HorizontalLayout::Builder(
      CodePointLayout::Builder('u'),
      VerticalOffsetLayout::Builder(
        LayoutHelper::String("n+1", 3),
        VerticalOffsetLayoutNode::Position::Subscript));
  }
  if (strcmp(m_name, "v(n)") == 0) {
    return HorizontalLayout::Builder(
        CodePointLayout::Builder('v'),
        VerticalOffsetLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayoutNode::Position::Subscript));
  }
  if (strcmp(m_name, "v(n+1)") == 0) {
    return HorizontalLayout::Builder(
      CodePointLayout::Builder('v'),
      VerticalOffsetLayout::Builder(
        LayoutHelper::String("n+1", 3),
          VerticalOffsetLayoutNode::Position::Subscript));
  }
  return LayoutHelper::String(m_name, strlen(m_name));
}

int SymbolNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return strlcpy(buffer, m_name, bufferSize);
}

Expression SymbolNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Symbol(this).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
}

Expression SymbolNode::shallowReplaceReplaceableSymbols(Context & context) {
  return Symbol(this).shallowReplaceReplaceableSymbols(context);
}

template<typename T>
Evaluation<T> SymbolNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Symbol s(this);
  Expression e = SymbolAbstract::Expand(s, context, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), context, complexFormat, angleUnit);
}

Expression Symbol::UntypedBuilder(const char * name, size_t length, Context * context) {
  // create an expression only if it is not in the context or defined as a symbol
  Symbol s = Symbol::Builder(name, length);
  if (SymbolAbstract::ValidInContext(s, context)) {
    return s;
  }
  return Expression();
}

bool SymbolNode::isUnknown(CodePoint unknownSymbol) const {
  bool result = UTF8Helper::CodePointIs(m_name, unknownSymbol);
  if (result) {
    assert(m_name[1] == 0);
  }
  return result;
}

Symbol Symbol::Builder(CodePoint name) {
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  int codePointLength = UTF8Decoder::CodePointToChars(name, buffer, bufferSize);
  assert(codePointLength < bufferSize);
  buffer[codePointLength] = 0;
  return Symbol::Builder(buffer, codePointLength);
}

bool Symbol::isSeriesSymbol(const char * c) {
  // [NV][1-3]
  if (c[2] == 0 && (c[0] == 'N' || c[0] == 'V') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

bool Symbol::isRegressionSymbol(const char * c) {
  // [XY][1-3]
  if (c[2] == 0 && (c[0] == 'X' || c[0] == 'Y') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

Expression Symbol::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  Expression parentExpression = parent();
  {
    Expression current = *this;
    Expression p = parentExpression;

    while (!p.isUninitialized()) {
      if (p.isParameteredExpression()) {
        int index = p.indexOfChild(current);
        if (index == ParameteredExpression::ParameterChildIndex()) {
          // The symbol is a paremetered expression's parameter
          return *this;
        }
        if (index == ParameteredExpression::ParameteredChildIndex()) {
          assert(p.childAtIndex(ParameteredExpression::ParameterChildIndex()).type() == ExpressionNode::Type::Symbol);
          Expression untypedParameter = p.childAtIndex(ParameteredExpression::ParameterChildIndex());
          Symbol parameter = static_cast<Symbol &>(untypedParameter);
          if (strcmp(parameter.name(), name()) == 0) {
            return *this;
          }
        }
      }
      current = p;
      p = current.parent();
    }
  }

  Symbol s = *this;
  Expression result = SymbolAbstract::Expand(s, context, true);
  if (result.isUninitialized()) {
    if (symbolicComputation) {
      return *this;
    }
    result = Undefined::Builder();
  }
  if (!parentExpression.isUninitialized()) {
    parentExpression.replaceChildInPlace(*this, result);
  }
  // The stored expression is as entered by the user, so we need to call reduce
  return result.deepReduce(context, complexFormat, angleUnit, target, symbolicComputation);
}

Expression Symbol::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  if (symbol.type() == ExpressionNode::Type::Symbol && strcmp(name(), symbol.name()) == 0) {
    Expression value = expression.clone();
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childNeedsParenthesis(value.node())) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

Expression Symbol::replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) {
  assert(!symbol.isUninitialized());
  assert(symbol.type() == ExpressionNode::Type::Symbol);
  assert(unknownSymbol.type() == ExpressionNode::Type::Symbol);
  return replaceSymbolWithExpression(symbol, unknownSymbol);
}

int Symbol::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  if (strcmp(name(), symbolName) == 0) {
    coefficients[0] = Rational::Builder(0);
    coefficients[1] = Rational::Builder(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

Expression Symbol::shallowReplaceReplaceableSymbols(Context & context) {
  if (isSystemSymbol()) {
    return *this;
  }
  Expression e = context.expressionForSymbol(*this, true);
  if (e.isUninitialized()) {
    return *this;
  }
  replaceWithInPlace(e);
  return e;
}

}
