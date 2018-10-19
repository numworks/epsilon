#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/sequence.h>

namespace Poincare {

class ProductNode final : public SequenceNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ProductNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Product";
  }
#endif

  Type type() const override { return Type::Product; }
private:
  float emptySequenceValue() const override { return 1.0f; }
  Layout createSequenceLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> Evaluation<T> templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b) const;
};

class Product final : public Expression {
friend class ProductNode;
public:
  Product();
  Product(const ProductNode * n) : Expression(n) {}
  Product(Expression operand0, Expression operand1, Expression operand2, Expression operand3) : Product() {
    replaceChildAtIndexInPlace(0, operand0);
    replaceChildAtIndexInPlace(1, operand1);
    replaceChildAtIndexInPlace(2, operand2);
    replaceChildAtIndexInPlace(3, operand3);
  }
  static const char * Name() { return "product"; }
  static const int NumberOfChildren() { return 3; }
};

}

#endif
