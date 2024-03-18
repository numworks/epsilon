#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/multiplication.h>
#include <poincare/sum_and_product.h>

namespace Poincare {

class ProductNode final : public SumAndProductNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(ProductNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Product"; }
#endif

  Type type() const override { return Type::Product; }

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
  static Product Builder(Expression argument, Symbol symbol,
                         Expression subScript, Expression superScript) {
    return TreeHandle::FixedArityBuilder<Product, ProductNode>(
        {argument, symbol, subScript, superScript});
  }
  static Expression UntypedBuilder(Expression children);

  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("product", 4, &UntypedBuilder);
};

}  // namespace Poincare

#endif
