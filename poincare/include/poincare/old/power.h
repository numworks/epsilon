#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include "approximation_helper.h"
#include "multiplication.h"
#include "rational.h"

namespace Poincare {

class Power;

class PowerNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(PowerNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Power"; }
#endif

  // Complex
  bool isReal(Context* context, bool canContainMatrices = true) const;

  // Properties
  Type otype() const override { return Type::Power; }
  OMG::Troolean isPositive(Context* context) const override;
  OMG::Troolean isNull(Context* context) const override;
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;
  double degreeForSortingAddition(bool symbolsOnly) const override;
  OExpression removeUnit(OExpression* unit) override;

  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;

 private:
  constexpr static int k_maxApproximatePowerMatrix = 1000;

  // Layout

  // Serialize
  bool childNeedsSystemParenthesesAtSerialization(
      const PoolObject* child) const override;
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplify
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  }
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  int simplificationOrderGreaterType(const ExpressionNode* e, bool ascending,
                                     bool ignoreParentheses) const override;
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
  // Evaluation
  template <typename T>
  static MatrixComplex<T> computeOnMatrixAndComplex(
      const MatrixComplex<T> m, const std::complex<T> d,
      Preferences::ComplexFormat complexFormat);
};

class Power final : public ExpressionTwoChildren<Power, PowerNode> {
  friend class ExpressionNode;
  friend class PowerNode;
  friend class Round;

 public:
  using ExpressionBuilder::ExpressionBuilder;

  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const;
  OExpression shallowBeautify(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  enum class DependencyType : uint8_t {
    None = 0,
    NegativeIndex,
    RationalIndex,
    Both
  };

  DependencyType typeOfDependency(
      const ReductionContext& reductionContext) const;
  static void AddPowerToListOfDependenciesIfNeeded(
      OExpression e, Power compareTo, OList l,
      const ReductionContext& reductionContext, bool clone);

  constexpr static AliasesList k_exponentialName = "exp";
  static OExpression ExponentialBuilder(OExpression children);
  constexpr static OExpression::FunctionHelper s_exponentialFunctionHelper =
      OExpression::FunctionHelper(k_exponentialName, 1, &ExponentialBuilder);

  static OExpression ChainedPowerBuilder(OExpression leftSide,
                                         OExpression rightSide);

 private:
  constexpr static int k_maxExactPowerMatrix = 100;
  constexpr static int k_maxNumberOfTermsInExpandedMultinome = 25;

  // Simplification
  static OExpression SafePowerRationalRational(
      const Rational base, const Rational index,
      const ReductionContext& reductionContext);
  // WARNING: These two methods alter their arguments
  static OExpression UnsafePowerRationalRational(
      Rational base, Rational index, const ReductionContext& reductionContext);
  static OExpression UnsafePowerIntegerRational(
      Integer base, Rational index, const ReductionContext& reductionContext);
  // Returns e^(i*pi*r)
  static OExpression CreateComplexExponent(
      const OExpression& r, const ReductionContext& reductionContext);
  static bool RationalExponentShouldNotBeReduced(const Rational& b,
                                                 const Rational& r);
  static bool IsLogarithmOfBase(const OExpression e, const OExpression base);
  static OExpression ReduceLogarithmLinearCombination(
      const ReductionContext& reductionContext, OExpression linearCombination,
      const OExpression baseOfLogarithmToReduce);
  static OExpression MinusOnePowerRational(
      const Rational index, const ReductionContext& reductionContext);
  bool isLogarithmOfSameBase(OExpression e) const;
  bool isNthRootOfUnity() const;

  // OUnit
  OExpression removeUnit(OExpression* unit);
};

}  // namespace Poincare

#endif
