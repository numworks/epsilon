#ifndef POINCARE_ARC_COTANGENT_H
#define POINCARE_ARC_COTANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class ArcCotangentNode final : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "arccot";

  // TreeNode
  size_t size() const override { return sizeof(ArcCotangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ArcCotangent";
  }
#endif

  // Properties
  Type type() const override { return Type::ArcCotangent; }

  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  
  // Derivation
   bool derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) override;
   Expression unaryFunctionDifferential(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class ArcCotangent final : public ExpressionOneChild<ArcCotangent, ArcCotangentNode> {
public:
   using ExpressionBuilder::ExpressionBuilder;

   // Derivation
   bool derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue);
   Expression unaryFunctionDifferential(const ReductionContext& reductionContext);
};

}

#endif