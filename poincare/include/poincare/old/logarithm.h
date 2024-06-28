#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include "addition.h"
#include "complex.h"
#include "expression_node_with_up_to_two_children.h"
#include "integer.h"
#include "old_expression.h"

namespace Poincare {

class LogarithmNode final : public ExpressionNodeWithOneOrTwoChildren {
 public:
  constexpr static AliasesList k_functionName = "log";

  // PoolObject
  size_t size() const override { return sizeof(LogarithmNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Logarithm";
  }
#endif

  // Properties
  Type otype() const override { return Type::Logarithm; }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext) override;
};

class Logarithm final
    : public ExpressionUpToTwoChildren<Logarithm, LogarithmNode> {
  friend class ExpressionNode;
  friend class LogarithmNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
  OExpression shallowBeautify();
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);

 private:
  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression simpleShallowReduce(const ReductionContext& reductionContext);
  Integer simplifyLogarithmIntegerBaseInteger(Integer i, Integer& base,
                                              Addition& a, bool isDenominator);
  OExpression splitLogarithmInteger(Integer i, bool isDenominator,
                                    const ReductionContext& reductionContext);
  bool parentIsAPowerOfSameBase() const;
};

}  // namespace Poincare

#endif
