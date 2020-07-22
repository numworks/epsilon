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

int SymbolNode::polynomialDegree(Context * context, const char * symbolName) const {
  if (strcmp(m_name, symbolName) == 0) {
    return 1;
  }
  return 0;
}

int SymbolNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  return Symbol(this).getPolynomialCoefficients(context, symbolName, coefficients, symbolicComputation);
}

int SymbolNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  // variables is in fact of type char[k_maxNumberOfVariables][maxSizeVariable]
  if (isVariable(m_name, context)) {
    int index = 0;
    while (index < maxSizeVariable*Expression::k_maxNumberOfVariables && variables[index] != 0) {
      if (strcmp(m_name, &variables[index]) == 0) {
        return nextVariableIndex;
      }
      index+= maxSizeVariable;
    }
    if (nextVariableIndex < Expression::k_maxNumberOfVariables) {
      assert(variables[nextVariableIndex*maxSizeVariable] == 0);
      if (strlen(m_name) + 1 > (size_t)maxSizeVariable) {
        return -2;
      }
      strlcpy(&variables[nextVariableIndex*maxSizeVariable], m_name, maxSizeVariable);
      nextVariableIndex++;
      if (nextVariableIndex < Expression::k_maxNumberOfVariables) {
        variables[nextVariableIndex*maxSizeVariable] = 0;
      }
      return nextVariableIndex;
    }
    return -1;
  }
  return nextVariableIndex;
}

Layout SymbolNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(!isUnknown());
  return LayoutHelper::String(m_name, strlen(m_name));
}

Expression SymbolNode::shallowReduce(ReductionContext reductionContext) {
  return Symbol(this).shallowReduce(reductionContext);
}

Expression SymbolNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return Symbol(this).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
}

ExpressionNode::LayoutShape SymbolNode::leftLayoutShape() const {
  UTF8Decoder decoder(m_name);
  decoder.nextCodePoint();
  if (decoder.nextCodePoint() == UCodePointNull) {  // nextCodePoint asserts that the first character is non-null
    return LayoutShape::OneLetter;
  }
  return LayoutShape::MoreLetters;
}

bool SymbolNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return Symbol(this).derivate(reductionContext, symbol, symbolValue);
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

bool SymbolNode::isUnknown() const {
  bool result = UTF8Helper::CodePointIs(m_name, UCodePointUnknown);
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

bool Symbol::isSeriesSymbol(const char * c, Poincare::Context * context) {
  // [NV][1-3]
  if (c[2] == 0 && (c[0] == 'N' || c[0] == 'V') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

bool Symbol::isRegressionSymbol(const char * c, Poincare::Context * context) {
  // [XY][1-3]
  if (c[2] == 0 && (c[0] == 'X' || c[0] == 'Y') && c[1] >= '1' && c[1] <= '3') {
    return true;
  }
  return false;
}

Expression Symbol::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
    || reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol)
  {
    return *this;
  }
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }
  {
    Expression current = *this;
    Expression p = parent();
    while (!p.isUninitialized()) {
      if (p.isParameteredExpression()) {
        int index = p.indexOfChild(current);
        if (index == ParameteredExpression::ParameterChildIndex()) {
          // The symbol is a paremetered expression's parameter
          return *this;
        }
        if (index == ParameteredExpression::ParameteredChildIndex()
            && hasSameNameAs(static_cast<ParameteredExpression&>(p).parameter()))
        {
          return *this;
        }
      }
      current = p;
      p = current.parent();
    }
  }

  Expression result = SymbolAbstract::Expand(*this, reductionContext.context(), true);
  if (result.isUninitialized()) {
    if (reductionContext.symbolicComputation() != ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined) {
      return *this;
    }
    result = Undefined::Builder();
  }
  replaceWithInPlace(result);
  // The stored expression is as entered by the user, so we need to call reduce
  return result.deepReduce(reductionContext);
}

bool Symbol::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  replaceWithInPlace(Rational::Builder(strcmp(name(), symbol.convert<Symbol>().name()) == 0));
  return true;
}

Expression Symbol::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  if (symbol.type() == ExpressionNode::Type::Symbol && hasSameNameAs(symbol)) {
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

int Symbol::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  if (strcmp(name(), symbolName) == 0) {
    coefficients[0] = Rational::Builder(0);
    coefficients[1] = Rational::Builder(1);
    return 1;
  }
  coefficients[0] = clone();
  return 0;
}

Expression Symbol::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  if (replaceFunctionsOnly || isSystemSymbol()) {
    return *this;
  }

  // Check that this is not a parameter in a parametered expression
  Expression ancestor = *this;
  while (parameteredAncestorsCount > 0) {
    ancestor = ancestor.parent();
    assert(!ancestor.isUninitialized());
    if (ancestor.isParameteredExpression()) {
      parameteredAncestorsCount--;
      Symbol ancestorParameter = static_cast<ParameteredExpression&>(ancestor).parameter();
      if (hasSameNameAs(ancestorParameter)) {
        return *this;
      }
    }
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
  *didReplace = true;
  return e;
}

}
