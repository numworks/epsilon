#ifndef POINCARE_FUNCTION_H
#define POINCARE_FUNCTION_H

#include "symbol_abstract.h"

namespace Poincare {

class FunctionNode final : public SymbolAbstractNode {
 public:
  using SymbolAbstractNode::SymbolAbstractNode;

  // PoolObject
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Function";
  }
#endif

  // Properties
  Type otype() const override { return Type::Function; }
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;
  int getVariables(Context* context, isVariableTest isVariable, char* variables,
                   int maxSizeVariable, int nextVariableIndex) const override;

 private:
  size_t nodeSize() const override { return sizeof(FunctionNode); }
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Function final : public SymbolAbstract {
  friend class FunctionNode;

 public:
  Function(const FunctionNode* n) : SymbolAbstract(n) {}
  static Function Builder(const char* name, size_t length,
                          JuniorExpression child);

  // Simplification
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
