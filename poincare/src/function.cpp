#include <poincare/function.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

FunctionNode::FunctionNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

bool FunctionNode::isReal(Context & context) const {
  Function f(this);
  return SymbolAbstract::isReal(f, context);
}

Expression FunctionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Function(this).replaceSymbolWithExpression(symbol, expression);
}

int FunctionNode::polynomialDegree(Context & context, const char * symbolName) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.polynomialDegree(context, symbolName);
}

int FunctionNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return -1;
  }
  return e.getPolynomialCoefficients(context, symbolName, coefficients);
}

int FunctionNode::getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return 0;
  }
  return e.getVariables(context, isVariable, variables, maxSizeVariable);
}

float FunctionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f,context, true);
  if (e.isUninitialized()) {
    return 0.0f;
  }
  return e.characteristicXRange(context, angleUnit);
}

Layout FunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Function(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int FunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FunctionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Function(this).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation); // This uses Symbol::shallowReduce
}

Expression FunctionNode::shallowReplaceReplaceableSymbols(Context & context) {
  return Function(this).shallowReplaceReplaceableSymbols(context);
}

Evaluation<float> FunctionNode::approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<float>(context, complexFormat, angleUnit);
}

Evaluation<double> FunctionNode::approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<double>(context, complexFormat, angleUnit);
}

template<typename T>
Evaluation<T> FunctionNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Function f(this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), context, complexFormat, angleUnit);
}

Function Function::Builder(const char * name, size_t length, Expression child) {
  Function f = SymbolAbstract::Builder<Function, FunctionNode>(name, length);
  if (!child.isUninitialized()) {
    f.replaceChildAtIndexInPlace(0, child);
  }
  return f;
}

Expression Function::UntypedBuilder(const char * name, size_t length, Expression child, Context * context) {
  /* Create an expression only if it is not in the context or defined as a
   * function */
  Function f = Function::Builder(name, length, child);
  if (SymbolAbstract::ValidInContext(f, context)) {
    return f;
  }
  return Expression();
}

Expression Function::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  // Replace the symbol in the child
  childAtIndex(0).replaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == ExpressionNode::Type::Function && strcmp(name(), symbol.name()) == 0) {
    Expression value = expression.clone();
    // Replace the unknown in the new expression by the function's child
    Symbol xSymbol = Symbol::Builder(UCodePointUnknownX);
    Expression xValue = childAtIndex(0);
    value = value.replaceSymbolWithExpression(xSymbol, xValue);
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childNeedsParenthesis(value.node())) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

Expression Function::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  Function f(*this);
  Expression e = SymbolAbstract::Expand(f, context, true);
  if (!e.isUninitialized()) {
    replaceWithInPlace(e);
    return e.deepReduce(context, complexFormat, angleUnit, target, symbolicComputation);
  }
  if (!symbolicComputation) {
    Expression result = Undefined::Builder();
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

Expression Function::shallowReplaceReplaceableSymbols(Context & context) {
  Expression e = context.expressionForSymbol(*this, true);
  if (e.isUninitialized()) {
    return *this;
  }
  e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknownX), childAtIndex(0));
  replaceWithInPlace(e);
  return e;
}

// TODO: should we avoid replacing unknown X in-place but use a context instead?
#if 0
VariableContext Function::unknownXContext(Context & parentContext) const {
  Symbol unknownXSymbol = Symbol::Builder(UCodePointUnknownX);
  Expression child = childAtIndex(0);
  const char x[] = {UCodePointUnknownX, 0}; // UGLY, use decoder
  /* COMMENT */
  if (child.type() == ExpressionNode::Type::Symbol && static_cast<Symbol &>(child).isSystemSymbol()) {
    return parentContext;
  }

  VariableContext xContext = VariableContext(x, &parentContext);

  /* If the parentContext already has an expression for UnknownX, we have to
   * replace in childAtIndex(0) any occurence of UnknownX by its value in
   * parentContext. That way, we handle: evaluatin f(x-1) with x = 2 & f:x->x^2 */
  Expression unknownXValue = parentContext.expressionForSymbol(unknownXSymbol, true);
  if (!unknownXValue.isUninitialized()) {
    xContext = static_cast<VariableContext &>(parentContext); // copy the parentContext
    child.replaceSymbolWithExpression(unknownXSymbol, unknownXValue);
  }
  /* We here assert that child contains no occurrence of UnknownX to avoid
   * creating an infinite loop (for instance: unknownXSymbol = unknownXSymbol+2). */
  assert(!child.recursivelyMatches([](const Expression e, Context & context, bool replaceSymbol) {
        return e.type() == ExpressionNode::Type::Symbol && static_cast<const Symbol &>(e).isSystemSymbol();
      }, parentContext, false));
  xContext.setExpressionForSymbol(child, unknownXSymbol, xContext);
  return xContext;
}
#endif

}
