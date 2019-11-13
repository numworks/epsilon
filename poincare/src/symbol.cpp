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

static inline int minInt(int x, int y) { return x < y ? x : y; }

constexpr char Symbol::k_ans[];

SymbolNode::SymbolNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

Expression SymbolNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Symbol(this).replaceSymbolWithExpression(symbol, expression);
}

int SymbolNode::polynomialDegree(Context * context, const char * symbolName) const {
  if (strcmp(m_name, symbolName) == 0) {
    return 1;
  }
  return 0;
}

int SymbolNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  return Symbol(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

int SymbolNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
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

float SymbolNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  return isUnknown(UCodePointUnknownX) ? NAN : 0.0f;
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(!isUnknown(UCodePointUnknownX));
  // TODO return Parse(m_name).createLayout() ?
  // Special case for the symbol names: u(n), u(n+1), v(n), v(n+1), w(n), w(n+1)
  const char * sequenceIndex[] = {"n", "n+1"};
  for (char sequenceName = 'u'; sequenceName <= 'w'; sequenceName++) {
    if (m_name[0] == sequenceName) {
      for (size_t i = 0; i < sizeof(sequenceIndex)/sizeof(char *); i++) {
        size_t sequenceIndexLength = strlen(sequenceIndex[i]);
        if (m_name[1] == '(' && strncmp(sequenceIndex[i], m_name+2, sequenceIndexLength) == 0 && m_name[2+sequenceIndexLength] == ')' && m_name[3+sequenceIndexLength] == 0) {
      return HorizontalLayout::Builder(
          CodePointLayout::Builder(sequenceName),
          VerticalOffsetLayout::Builder(
            LayoutHelper::String(sequenceIndex[i], sequenceIndexLength),
            VerticalOffsetLayoutNode::Position::Subscript));
        }
      }
    }
  }
  return LayoutHelper::String(m_name, strlen(m_name));
}

int SymbolNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return minInt(bufferSize - 1, strlcpy(buffer, m_name, bufferSize));
}

Expression SymbolNode::shallowReduce(ReductionContext reductionContext) {
  return Symbol(this).shallowReduce(reductionContext);
}

Expression SymbolNode::shallowReplaceReplaceableSymbols(Context * context) {
  return Symbol(this).shallowReplaceReplaceableSymbols(context);
}

ExpressionNode::LayoutShape SymbolNode::leftLayoutShape() const {
  UTF8Decoder decoder(m_name);
  decoder.nextCodePoint();
  if (decoder.nextCodePoint() == UCodePointNull) {  // nextCodePoint asserts that the first character is non-null
    return LayoutShape::OneLetter;
  }
  return LayoutShape::MoreLetters;
}

template<typename T>
Evaluation<T> SymbolNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Symbol s(this);
  Expression e = SymbolAbstract::Expand(s, context, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), context, complexFormat, angleUnit);
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

Expression Symbol::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
  Expression result = SymbolAbstract::Expand(s, reductionContext.context(), true);
  if (result.isUninitialized()) {
    if (reductionContext.symbolicComputation()) {
      return *this;
    }
    result = Undefined::Builder();
  }
  if (!parentExpression.isUninitialized()) {
    parentExpression.replaceChildInPlace(*this, result);
  }
  // The stored expression is as entered by the user, so we need to call reduce
  return result.deepReduce(reductionContext);
}

Expression Symbol::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  if (symbol.type() == ExpressionNode::Type::Symbol && strcmp(name(), symbol.name()) == 0) {
    Expression value = expression.clone();
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childAtIndexNeedsUserParentheses(value, p.indexOfChild(*this))) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

int Symbol::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  if (strcmp(name(), symbolName) == 0) {
    coefficients[0] = Rational::Builder(0);
    coefficients[1] = Rational::Builder(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

Expression Symbol::shallowReplaceReplaceableSymbols(Context * context) {
  if (isSystemSymbol()) {
    return *this;
  }
  Expression e = context->expressionForSymbolAbstract(*this, true);
  if (e.isUninitialized()) {
    return *this;
  }
  // If the symbol contains itself, return undefined
  if (e.hasExpression([](Expression e, const void * context) {
          if (e.type() != ExpressionNode::Type::Symbol) {
            return false;
          }
          return strcmp(static_cast<Symbol&>(e).name(), reinterpret_cast<const char *>(context)) == 0;
        }, reinterpret_cast<const void *>(name())))
  {
    return replaceWithUndefinedInPlace();
  }
  replaceWithInPlace(e);
  return e;
}

}
