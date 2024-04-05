#ifndef POINCARE_LIST_ACCESS_H
#define POINCARE_LIST_ACCESS_H

#include <poincare/old_expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>

namespace Poincare {

template <int U>
class ListAccessNode : public ExpressionNode {
 public:
  constexpr static int k_listChildIndex = U;

  size_t size() const override { return sizeof(ListAccessNode); }
  int numberOfChildren() const override { return U + 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << (U == 1 ? "ListElement" : "ListSlice");
  }
#endif
  Type otype() const override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };

 private:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class ListElement : public OExpression {
 public:
  ListElement(const ListAccessNode<1>* n) : OExpression(n) {}
  static ListElement Builder(OExpression index, OExpression list) {
    return PoolHandle::FixedArityBuilder<ListElement, ListAccessNode<1>>(
        {index, list});
  }

  OExpression shallowReduce(ReductionContext reductionContext);
};

class ListSlice : public OExpression {
 public:
  ListSlice(const ListAccessNode<2>* n) : OExpression(n) {}
  static ListSlice Builder(OExpression firstIndex, OExpression lastIndex,
                           OExpression list) {
    return PoolHandle::FixedArityBuilder<ListSlice, ListAccessNode<2>>(
        {firstIndex, lastIndex, list});
  }

  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
