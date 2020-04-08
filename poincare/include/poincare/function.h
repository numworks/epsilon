#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/symbol_abstract.h>
#include <poincare/variable_context.h>

namespace Poincare {

class FunctionNode : public SymbolAbstractNode  {
public:
  FunctionNode(const char * newName, int length);

  // SymbolAbstractNode
  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 1; } //TODO allow any number of children? Needs templating
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Function";
  }
#endif

  // Properties
  Type type() const override { return Type::Function; }
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const override;
  int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const override;
  float characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const override;
  /* getUnit() is ExpressionNode::getUnit ->
   * as the function is reduced, it would have been replaced if it had a
   * definition. It thus has no definition, so no unit. */

private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(FunctionNode); }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) override;
  LayoutShape leftLayoutShape() const override { return strlen(m_name) > 1 ? LayoutShape::MoreLetters : LayoutShape::OneLetter; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override;
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override;
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Function : public SymbolAbstract {
friend class FunctionNode;
public:
  Function(const FunctionNode * n) : SymbolAbstract(n) {}
  static Function Builder(const char * name, size_t length, Expression child = Expression());

  // Simplification
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount);
};

}

#endif
