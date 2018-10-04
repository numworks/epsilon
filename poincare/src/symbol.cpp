#include <poincare/symbol.h>
#include <poincare/char_layout.h>
#include <poincare/context.h>
#include <poincare/helpers.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/vertical_offset_layout.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

constexpr char Symbol::k_ans[];

void SymbolNode::initToMatchSize(size_t goalSize) {
  assert(goalSize != sizeof(SymbolNode));
  int nameSize = goalSize - sizeof(SymbolNode);
  for (int i = 0; i < nameSize - 1; i++) {
    m_name[i] = 'a';
  }
  m_name[nameSize-1] = 0;
  assert(size() == goalSize);
}

/* TreePool uses adresses and sizes that are multiples of 4 in order to make
 * node moves faster.*/
static size_t NodeSize(size_t nameLength) {
  return Helpers::AlignedSize(sizeof(SymbolNode)+nameLength+1, 4);
}

size_t SymbolNode::size() const {
  return NodeSize(strlen(m_name));
}

ExpressionNode::Sign SymbolNode::sign() const {
  /* TODO: Maybe, we will want to know that from a context given in parameter:
  if (context.expressionForSymbol(this) != nullptr) {
    return context.expressionForSymbol(this)->sign(context);
  }*/
  if (isPi()) {
    return Sign::Positive;
  }
  if (isExponential()) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

Expression SymbolNode::replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
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
  return 0.0;
}

int SymbolNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Symbol);
  return strcmp(m_name, static_cast<const SymbolNode *>(e)->name());
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_name[0] == Symbol::SpecialSymbols::UnknownX) {
    assert(m_name[1] == 0);
    return CharLayout('x');
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

Expression SymbolNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Symbol(this).shallowReduce(context, angleUnit, replaceSymbols);
}

bool SymbolNode::shouldBeReplacedWhileReducing(Context & context) const {
  if (isIComplex() || isPi() || isExponential()) {
    assert(m_name[1] == 0);
    return false;
  }
  return true;
}

template<typename T>
Evaluation<T> SymbolNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  if (isIComplex()) {
    assert(m_name[1] == 0);
    return Complex<T>(0.0, 1.0);
  }
  const Expression e = context.expressionForSymbol(Symbol(this));
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.approximateToEvaluation<T>(context, angleUnit);
}

Symbol::Symbol(const char * name, int length) : Expression(TreePool::sharedPool()->createTreeNode<SymbolNode>(NodeSize(length))) {
  node()->setName(name, length);
}

Symbol::Symbol(char name) : Symbol(&name, 1) {}

bool Symbol::isSeriesSymbol(const char * c) {
  // [NV][1-3]
  if ((c[0] == 'N' || c[0] == 'V') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

bool Symbol::isRegressionSymbol(const char * c) {
  // [XY][1-3]
  if ((c[0] == 'X' || c[0] == 'Y') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

bool Symbol::matches(ExpressionTest test, Context & context) const {
  Expression e = context.expressionForSymbol(*this);
  return !e.isUninitialized() && test(e, context);
}

Expression Symbol::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  assert(strcmp(name(), k_ans) != 0);
  /* Do not replace symbols in expression of type: var+3->A. Store needs to
   * replace unknown variables first. */
  Expression p = parent();
  if (!replaceSymbols ||(!p.isUninitialized() && p.type() == ExpressionNode::Type::Store)) {
    return *this;
  }
  const Expression e = context.expressionForSymbol(*this);
  if (!e.isUninitialized() && node()->shouldBeReplacedWhileReducing(context)) {
    // The stored expression had been beautified, so we need to call deepReduce
    Expression result = e.clone();
    replaceWithInPlace(result);
    return result.deepReduce(context, angleUnit);
  }
  return *this;
}

Expression Symbol::replaceSymbolWithExpression(const Symbol & symbol, const Expression & expression) {
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

int Symbol::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  if (strcmp(name(), symbolName) == 0) {
    coefficients[0] = Rational(0);
    coefficients[1] = Rational(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

}
