#ifndef POINCARE_MULTIPLICATION_EXPLICIT_H
#define POINCARE_MULTIPLICATION_EXPLICIT_H

#include <poincare/multiplication.h>
#include <poincare/n_ary_expression.h>

namespace Poincare {

class MultiplicationExplicitNode /*final*/ : public MultiplicationNode {
  friend class Addition;
public:
  using MultiplicationNode::MultiplicationNode;
  // Tree
  size_t size() const override { return sizeof(MultiplicationExplicitNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Multiplication Explicit";
  }
#endif

  // Properties
  Type type() const override { return Type::MultiplicationExplicit; }

private:
  // Property
  Expression setSign(Sign s, ReductionContext reductionContext) override;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Serialize
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext reductionContext) override;
  Expression denominator(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override;

};

class MultiplicationExplicit : public Multiplication {
  friend class AdditionNode;
  friend class Addition;
  friend class Power;
public:
  MultiplicationExplicit(const MultiplicationExplicitNode * n) : Multiplication(n) {}
  static MultiplicationExplicit Builder() { return TreeHandle::NAryBuilder<MultiplicationExplicit, MultiplicationExplicitNode>(); }
  static MultiplicationExplicit Builder(Expression e1) { return MultiplicationExplicit::Builder(&e1, 1); }
  static MultiplicationExplicit Builder(Expression e1, Expression e2) { return MultiplicationExplicit::Builder(ArrayBuilder<Expression>(e1, e2).array(), 2); }
  static MultiplicationExplicit Builder(Expression e1, Expression e2, Expression e3) { return MultiplicationExplicit::Builder(ArrayBuilder<Expression>(e1, e2, e3).array(), 3); }
  static MultiplicationExplicit Builder(Expression e1, Expression e2, Expression e3, Expression e4) { return MultiplicationExplicit::Builder(ArrayBuilder<Expression>(e1, e2, e3, e4).array(), 4); }
  static MultiplicationExplicit Builder(Expression * children, size_t numberOfChildren) { return TreeHandle::NAryBuilder<MultiplicationExplicit, MultiplicationExplicitNode>(children, numberOfChildren); }

  Expression setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);
  Expression omitMultiplicationWhenPossible();
  Expression denominator(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  void sortChildrenInPlace(NAryExpressionNode::ExpressionOrder order, Context * context, bool canBeInterrupted) {
    NAryExpression::sortChildrenInPlace(order, context, false, canBeInterrupted);
  }
private:
  // Simplification
  Expression privateShallowReduce(ExpressionNode::ReductionContext reductionContext, bool expand, bool canBeInterrupted);
  void mergeMultiplicationChildrenInPlace();
  void factorizeBase(int i, int j, ExpressionNode::ReductionContext reductionContext);
  void mergeInChildByFactorizingBase(int i, Expression e, ExpressionNode::ReductionContext reductionContext);
  void factorizeExponent(int i, int j, ExpressionNode::ReductionContext reductionContext);
  Expression distributeOnOperandAtIndex(int index, ExpressionNode::ReductionContext reductionContext);
  void addMissingFactors(Expression factor, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  void factorizeSineAndCosine(int i, int j, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  static bool HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalBase(const Expression & e1, const Expression & e2);
  static bool TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2);
  static bool TermHasNumeralBase(const Expression & e);
  static bool TermHasNumeralExponent(const Expression & e);
  static const Expression CreateExponent(Expression e);
  /* Warning: mergeNegativePower doesnot always return  a multiplication:
   *      *(b^-1,c^-1) -> (bc)^-1 */
  Expression mergeNegativePower(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  static inline const Expression Base(const Expression e);
};

}

#endif
