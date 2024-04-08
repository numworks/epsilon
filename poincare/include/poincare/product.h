#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include "multiplication.h"
#include "sum_and_product.h"

namespace Poincare {

class ProductNode final : public SumAndProductNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(ProductNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Product"; }
#endif

  Type otype() const override { return Type::Product; }

 private:
  float emptySumAndProductValue() const override { return 1.0f; }
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<double> evaluateWithNextTerm(
      DoublePrecision p, Evaluation<double> a, Evaluation<double> b,
      Preferences::ComplexFormat complexFormat) const override {
    return MultiplicationNode::Compute<double>(a, b, complexFormat);
  }
  Evaluation<float> evaluateWithNextTerm(
      SinglePrecision p, Evaluation<float> a, Evaluation<float> b,
      Preferences::ComplexFormat complexFormat) const override {
    return MultiplicationNode::Compute<float>(a, b, complexFormat);
  }
};

class Product final : public SumAndProduct {
  friend class ProductNode;

 public:
  Product(const ProductNode* n) : SumAndProduct(n) {}
  static Product Builder(OExpression argument, Symbol symbol,
                         OExpression subScript, OExpression superScript) {
    return PoolHandle::FixedArityBuilder<Product, ProductNode>(
        {argument, symbol, subScript, superScript});
  }
  static OExpression UntypedBuilder(OExpression children);

  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("product", 4, &UntypedBuilder);
};

}  // namespace Poincare

#endif
