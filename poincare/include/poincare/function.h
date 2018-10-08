#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include <poincare/symbol_abstract.h>
#include <poincare/variable_context.h>

namespace Poincare {

class FunctionNode : public SymbolAbstractNode  {
public:
  FunctionNode() :
    SymbolAbstractNode(),
    m_color(KDColorRed),
    m_displayDerivative(true),
    m_isActive(true)
  {}

  // FunctionNode
  KDColor color() const { return m_color; }
  bool displayDerivative() const { return m_displayDerivative; }
  bool isActive() const { return m_isActive; }
  void setColor(KDColor color) { m_color = color; }
  void setDisplayDerivative(bool display) { m_displayDerivative = display; }
  void setActive (bool active) { m_isActive = active; }

  // SymbolAbstractNode
  const char * name() const override { return m_name; }

  // TreeNode
  void initToMatchSize(size_t goalSize) override;
  size_t size() const override;
  int numberOfChildren() const override { return 1; } //TODO allow any number of children? Needs templating
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Function";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " name=\"" << m_name << "\"";
    stream << " color=\"" << m_color << "\"";
    stream << " displayDerivative=\"" << m_displayDerivative << "\"";
    stream << " active=\"" << m_isActive << "\"";
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
  KDColor m_color;
  bool m_displayDerivative;
  bool m_isActive;
  char m_name[0]; // MUST be the last member variable

  VariableContext xContext(Context & parentContext) const;
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override;
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override;
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Function : public SymbolAbstract {
public:
  explicit Function(const char * name);
  Function(const FunctionNode * n) : SymbolAbstract(n) {}
  explicit Function(const char * name, Expression operand) : Function(name) {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
