#include <poincare/symbol.h>
#include <poincare/char_layout.h>
#include <poincare/context.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/vertical_offset_layout.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

constexpr char Symbol::k_ans[];

/*ExpressionNode::Sign SymbolNode::sign() const {
  TODO: Maybe, we will want to know that from a context given in parameter:
  if (context.expressionForSymbol(this, false) != nullptr) {
    return context.expressionForSymbol(this, false)->sign(context);
  }
}
*/

Expression SymbolNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
}

Expression SymbolNode::replaceUnknown(const Symbol & symbol) {
  return Symbol(this).replaceUnknown(symbol);
}

int SymbolNode::polynomialDegree(Context & context, const char * symbolName) const {
  if (strcmp(m_name,symbolName) == 0) {
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
  if (m_name[0] == Symbol::SpecialSymbols::UnknownX) {
    assert(m_name[1] == 0);
    return NAN;
  }
  return 0.0f;
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_name[0] == Symbol::SpecialSymbols::UnknownX) {
    assert(m_name[1] == 0);
    return CharLayout(Symbol::k_unknownXReadableChar);
  }
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
  if (strcmp(m_name, "v(n)") == 0) {
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

Expression SymbolNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Symbol(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression SymbolNode::shallowReplaceReplaceableSymbols(Context & context) {
  return Symbol(this).shallowReplaceReplaceableSymbols(context);
}

template<typename T>
Evaluation<T> SymbolNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Symbol s(this);
  Expression e = SymbolAbstract::Expand(s, context, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.approximateToEvaluation<T>(context, angleUnit);
}

Symbol::Symbol(const char * name, int length) : SymbolAbstract(TreePool::sharedPool()->createTreeNode<SymbolNode>(SymbolAbstract::AlignedNodeSize(length, sizeof(SymbolNode)))) {
  node()->setName(name, length);
}

Symbol::Symbol(char name) : Symbol(&name, 1) {}

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

Expression Symbol::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  if (!replaceSymbols) {
    return *this;
  }
  Symbol s = *this;
  Expression result = SymbolAbstract::Expand(s, context, true);
  if (result.isUninitialized()) {
    return *this;
  }
  replaceWithInPlace(result);
  // The stored expression is as entered by the user, so we need to call reduce
  return result.deepReduce(context, angleUnit);
}

Expression Symbol::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  if (symbol.type() == ExpressionNode::Type::Symbol && strcmp(name(), symbol.name()) == 0) {
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

Expression Symbol::replaceUnknown(const Symbol & symbol) {
  assert(!symbol.isUninitialized());
  assert(symbol.type() == ExpressionNode::Type::Symbol);
  return replaceSymbolWithExpression(symbol, Symbol(SpecialSymbols::UnknownX));
}

int Symbol::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  if (strcmp(name(), symbolName) == 0) {
    coefficients[0] = Rational(0);
    coefficients[1] = Rational(1);
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
