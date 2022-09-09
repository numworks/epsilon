#ifndef POINCARE_COMPARISON_EXPRESSION_H
#define POINCARE_COMPARISON_EXPRESSION_H

#include <poincare/expression.h>

/* This class can have any number of children.
 * This implements "x = y", "1 < x <= y" and any other sequence of comparisons
 * like "x > 6 != 7 <= y = z".
 *
 * "a < b = c != d" is simplified as "a < b and b = c and c != d"*/

namespace Poincare {

class ComparisonNode : public ExpressionNode {
  friend class Comparison;
public:
  enum class OperatorType : uint8_t {
    Equal = 0,
    NotEqual,
    Superior,
    Inferior,
    SuperiorEqual,
    InferiorEqual,
    // Value to know size of enum. Do not use and leave at the end
    NumberOfTypes
  };
  static CodePoint ComparisonCodePoint(OperatorType type);
  static bool IsComparisonOperatorCodePoint(CodePoint c, OperatorType * returnType = nullptr);
  static bool IsComparisonOperatorString(const char * s, size_t length, OperatorType * returnType = nullptr);
  static OperatorType Reverse(OperatorType type);

  static bool IsBinaryComparison(Expression e, OperatorType * returnType = nullptr);
  static bool IsBinaryComparisonWithOperator(Expression e, OperatorType operatorType);
  static bool IsBinaryEquality(Expression e) { return IsBinaryComparisonWithOperator(e, OperatorType::Equal); }

  static TrinaryBoolean TruthValueOfOperator(OperatorType type, TrinaryBoolean chidlrenAreEqual, TrinaryBoolean leftChildIsGreater);

  //Tree
  size_t size() const override;
  int numberOfChildren() const override { return m_numberOfOperands; }

  Type type() const override { return Type::Comparison; }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Comparison";
  }
  void logAttributes(std::ostream & stream) const override;
#endif

  OperatorType operatorAtIndex(int i) { assert(i < numberOfOperators() && i >= 0); return m_operatorsList[i]; }
  OperatorType * listOfOperators() { return m_operatorsList; }

private:
  /* This constructor takes its last operator separately so that you can
   * copy another ComparisonNode and add an operator at the end of it. */
  ComparisonNode(int numberOfOperands, OperatorType * operatorsListButTheLast, OperatorType lastOperatorOfList);
  ComparisonNode(OperatorType operatorType) : ComparisonNode(2, nullptr, operatorType) {}

  int numberOfOperators() const { assert(m_numberOfOperands >= 2); return m_numberOfOperands - 1; }

  // Layout
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::BoundaryPunctuation; };
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override;
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;

  // Simplification
  Expression shallowReduce(const ReductionContext& ReductionContext) override;

  uint16_t m_numberOfOperands;
  /* This variable contains the list of operators to insert between the
   * operands. Example: in "a=b<c", the operators list is {=,<}. */
  OperatorType m_operatorsList[0]; // Must be last
};

class Comparison : public Expression {
public:
  Comparison(const ComparisonNode * n) : Expression(n) {}
  static Comparison Builder(Expression child0, ComparisonNode::OperatorType operatorType, Expression child1);
  Comparison addComparison(ComparisonNode::OperatorType operatorType, Expression child);

  Expression shallowReduce(ExpressionNode::ReductionContext ReductionContext);
  ComparisonNode::OperatorType operatorAtIndex(int i) const { return node()->operatorAtIndex(i); }
private:
  ComparisonNode * node() const { return static_cast<ComparisonNode *>(Expression::node()); }
};

}

#endif
