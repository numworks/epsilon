#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include "approximation_helper.h"
#include "n_ary_infix_expression.h"
#include "rational.h"

namespace Poincare {

class AdditionNode final : public NAryInfixExpressionNode {
  friend class Addition;

 public:
  using NAryInfixExpressionNode::NAryInfixExpressionNode;

  // Tree
  size_t size() const override { return sizeof(AdditionNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Addition";
  }
#endif

  // ExpressionNode

  // Properties
  Type otype() const override { return Type::Addition; }
  OMG::Troolean isPositive(Context* context) const override;
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;

  // Simplification
  LayoutShape leftLayoutShape() const override {
    /* When beautifying a Multiplication of Additions, Parentheses are added
     * around Additions. As leftLayoutShape is called after beautifying, we
     * should never call it on an Addition. */
    assert(false);
    return NAryExpressionNode::leftLayoutShape();
  }
  LayoutShape rightLayoutShape() const override {
    assert(false);
    return NAryExpressionNode::rightLayoutShape();
  }

 private:
  // Layout
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

class Addition final : public NAryExpression {
 public:
  Addition(const AdditionNode* n) : NAryExpression(n) {}
  static Addition Builder(const Tuple& children = {}) {
    return PoolHandle::NAryBuilder<Addition, AdditionNode>(convert(children));
  }
  // TODO: Get rid of these two helper functions
  static Addition Builder(OExpression e1) { return Addition::Builder({e1}); }
  static Addition Builder(OExpression e1, OExpression e2) {
    return Addition::Builder({e1, e2});
  }
  // OExpression
  OExpression shallowBeautify(const ReductionContext& reductionContext);
  /* WARNING: If the parent of an addition is also an addition (or a
   * subtraction), the reduction won't do anything and let the parent
   * do the reduction. So a child addition can't be reduced without
   * reducing its parent addition. */
  OExpression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const;
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order,
                           Context* context, bool canContainMatrices = true) {
    NAryExpression::sortChildrenInPlace(order, context, true,
                                        canContainMatrices);
  }

 private:
  static const Number NumeralFactor(const OExpression& e);
  static inline int NumberOfNonNumeralFactors(const OExpression& e);
  static inline const OExpression FirstNonNumeralFactor(const OExpression& e);

  static bool TermsHaveIdenticalNonNumeralFactors(const OExpression& e1,
                                                  const OExpression& e2,
                                                  Context* context);
  /* Return true if the expression is y*cos(x)^2 or y*sin(x)^2 and
   * "base" is unintialized or "base" == x.
   * Set "coefficient" to y. If "base" is uninitialized, set "base" to x
   * Set cosine to true if it's a cosine, to false if it's a sine. */
  static bool TermHasSquaredTrigFunctionWithBase(
      const OExpression& e, const ReductionContext& reductionContext,
      OExpression& base, OExpression& coefficient, bool* cosine);
  static bool TermHasSquaredCos(const OExpression& e,
                                const ReductionContext& reductionContext,
                                OExpression& baseOfCos);

  OExpression factorizeOnCommonDenominator(ReductionContext reductionContext);
  void factorizeChildrenAtIndexesInPlace(
      int index1, int index2, const ReductionContext& reductionContext);
  OExpression factorizeSquaredTrigFunction(
      OExpression& baseOfTrigFunction,
      const ReductionContext& reductionContext);
  AdditionNode* node() const {
    return static_cast<AdditionNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
