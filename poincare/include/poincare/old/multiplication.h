#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include "approximation_helper.h"
#include "n_ary_infix_expression.h"

namespace Poincare {

class MultiplicationNode final : public NAryInfixExpressionNode {
 public:
  using NAryInfixExpressionNode::NAryInfixExpressionNode;

  // Tree
  size_t size() const override { return sizeof(MultiplicationNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Multiplication";
  }
#endif

  // Properties
  Type otype() const override { return Type::Multiplication; }
  OMG::Troolean isPositive(Context* context) const override;
  OMG::Troolean isNull(Context* context) const override;
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;
  OExpression removeUnit(OExpression* unit) override;
  double degreeForSortingAddition(bool symbolsOnly) const override;

 private:
  enum class MultiplicationSymbol : uint8_t {
    // The order matters !
    Empty = 0,
    MiddleDot = 1,
    MultiplicationSign = 2,
  };

  static MultiplicationSymbol OperatorSymbolBetween(
      ExpressionNode::LayoutShape left, ExpressionNode::LayoutShape right);
  static CodePoint CodePointForOperatorSymbol(MultiplicationSymbol symbol);
  // Layout
  CodePoint operatorSymbol() const;

  // Serialize
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
};

class Multiplication : public NAryExpression {
  friend class Addition;
  friend class Power;
  friend class MultiplicationNode;

 public:
  Multiplication(const MultiplicationNode* n) : NAryExpression(n) {}
  static Multiplication Builder(const Tuple& children = {}) {
    return PoolHandle::NAryBuilder<Multiplication, MultiplicationNode>(
        convert(children));
  }
  // TODO: Get rid of those helpers
  static Multiplication Builder(OExpression e1) {
    return Multiplication::Builder({e1});
  }
  static Multiplication Builder(OExpression e1, OExpression e2) {
    return Multiplication::Builder({e1, e2});
  }
  static Multiplication Builder(OExpression e1, OExpression e2,
                                OExpression e3) {
    return Multiplication::Builder({e1, e2, e3});
  }
  static Multiplication Builder(OExpression e1, OExpression e2, OExpression e3,
                                OExpression e4) {
    return Multiplication::Builder({e1, e2, e3, e4});
  }

  // Properties
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const;

  // Simplification
  OExpression shallowBeautify(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order,
                           Context* context, bool canContainMatrices = true) {
    NAryExpression::sortChildrenInPlace(order, context, false,
                                        canContainMatrices);
  }
  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  void splitIntoNormalForm(OExpression& numerator, OExpression& denominator,
                           const ReductionContext& reductionContext) const;

 private:
  // OUnit
  OExpression removeUnit(OExpression* unit);

  // Simplification
  void factorizeBase(int i, int j, const ReductionContext& reductionContext,
                     OList dependenciesCreatedDuringReduction);
  void mergeInChildByFactorizingBase(
      int i, OExpression e, const ReductionContext& reductionContext,
      OList dependenciesCreatedDuringReduction = OList());
  bool factorizeExponent(int i, int j,
                         const ReductionContext& reductionContext);
  OExpression gatherLikeTerms(const ReductionContext& reductionContext);
  bool gatherRationalPowers(int i, int j,
                            const ReductionContext& reductionContext);
  OExpression distributeOnOperandAtIndex(
      int index, const ReductionContext& reductionContext);
  // factor must be a reduced expression
  void addMissingFactors(OExpression factor,
                         const ReductionContext& reductionContext);
  bool factorizeSineAndCosine(int i, int j,
                              const ReductionContext& reductionContext);
  static bool HaveSameNonNumeralFactors(const OExpression& e1,
                                        const OExpression& e2);
  static bool TermsHaveIdenticalBase(const OExpression& e1,
                                     const OExpression& e2);
  static bool TermsHaveIdenticalExponent(const OExpression& e1,
                                         const OExpression& e2);
  static bool TermHasNumeralBase(const OExpression& e);
  static bool TermHasNumeralExponent(const OExpression& e);
  static bool TermIsPowerOfRationals(const OExpression& e);
  static const OExpression CreateExponent(OExpression e);
  static inline const OExpression Base(const OExpression e);
};

}  // namespace Poincare

#endif
