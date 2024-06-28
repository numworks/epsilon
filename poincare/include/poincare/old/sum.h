#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include "addition.h"
#include "sum_and_product.h"

namespace Poincare {

class SumNode final : public SumAndProductNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(SumNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Sum"; }
#endif

  Type otype() const override { return Type::Sum; }

 private:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class Sum final : public SumAndProduct {
  friend class SumNode;

 public:
  Sum(const SumNode* n) : SumAndProduct(n) {}
  static Sum Builder(OExpression argument, Symbol symbol, OExpression subScript,
                     OExpression superScript) {
    return PoolHandle::FixedArityBuilder<Sum, SumNode>(
        {argument, symbol, subScript, superScript});
  }
  static OExpression UntypedBuilder(OExpression children);

  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("sum", 4, &UntypedBuilder);
};

}  // namespace Poincare

#endif
