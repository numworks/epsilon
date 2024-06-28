#ifndef POINCARE_IMAGINARY_PART_H
#define POINCARE_IMAGINARY_PART_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ImaginaryPartNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "im";

  // PoolObject
  size_t size() const override { return sizeof(ImaginaryPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ImaginaryPart";
  }
#endif

  // Properties
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isPositive(context) == OMG::Troolean::Unknown
               ? OMG::Troolean::Unknown
               : OMG::Troolean::True;
  }
  Type otype() const override { return Type::ImaginaryPart; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class ImaginaryPart final
    : public ExpressionOneChild<ImaginaryPart, ImaginaryPartNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
