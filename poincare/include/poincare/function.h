#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/symbol_abstract.h>
#include <poincare/variable_context.h>

namespace Poincare {

class FunctionNode : public SymbolAbstractNode  {
public:
  // SymbolAbstractNode
  const char * name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 1; } //TODO allow any number of children? Needs templating
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Function";
  }
#endif

  // Properties
  Type type() const override { return Type::Function; }
  Sign sign() const override { return Sign::Unknown; }
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
  int polynomialDegree(Context & context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const override;
  int getVariables(Context & context, isVariableTest isVariable, char * variables, int maxSizeVariable) const override;
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

private:
  char m_name[0]; // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(FunctionNode); }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression shallowReplaceReplaceableSymbols(Context & context) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override;
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override;
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Function : public SymbolAbstract {
friend class FunctionNode;
public:
  Function(const char * name, size_t length);
  Function(const FunctionNode * n) : SymbolAbstract(n) {}
  Function(const char * name, size_t length, Expression child) : Function(name, length) {
    replaceChildAtIndexInPlace(0, child);
  }
  static Expression UntypedBuilder(const char * name, size_t length, Expression child, Context * context) {
    /* Create an expression only if it is not in the context or defined as a
     * function */
    Function f(name, length, child);
    if (SymbolAbstract::ValidInContext(f, context)) {
      return f;
    }
    return Expression();
  }

  // Simplification
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression shallowReplaceReplaceableSymbols(Context & context);
private:
  //VariableContext unknownXContext(Context & parentContext) const;
};

}

#endif
