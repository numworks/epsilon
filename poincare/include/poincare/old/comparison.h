#ifndef POINCARE_COMPARISON_EXPRESSION_H
#define POINCARE_COMPARISON_EXPRESSION_H

#include <ion/unicode/utf8_decoder.h>

#include <array>

#include "junior_expression.h"
#include "old_expression.h"

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

  static const char* ComparisonOperatorString(OperatorType type);
  static Layout ComparisonOperatorLayout(OperatorType type);
  static bool IsComparisonOperatorString(const char* s, const char* stringEnd,
                                         OperatorType* returnType,
                                         size_t* returnLength);

  static OperatorType SwitchInferiorSuperior(OperatorType type);

  static bool IsBinaryComparison(JuniorExpression e,
                                 OperatorType* returnType = nullptr);
  static bool IsBinaryComparisonWithOperator(JuniorExpression e,
                                             OperatorType operatorType);
  static bool IsBinaryEquality(JuniorExpression e) {
    return IsBinaryComparisonWithOperator(e, OperatorType::Equal);
  }
  static bool IsComparisonWithoutNotEqualOperator(
      OExpression e);  // Return false if one of the operator is NotEqual

  static TrinaryBoolean TruthValueOfOperator(OperatorType type,
                                             TrinaryBoolean chidlrenAreEqual,
                                             TrinaryBoolean leftChildIsGreater);

  // Tree
  size_t size() const override;
  int numberOfChildren() const override { return m_numberOfOperands; }

  Type otype() const override { return Type::Comparison; }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Comparison";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  OperatorType operatorAtIndex(int i) {
    assert(i < numberOfOperators() && i >= 0);
    return m_operatorsList[i];
  }
  OperatorType* listOfOperators() { return m_operatorsList; }

  int numberOfOperators() const {
    assert(m_numberOfOperands >= 2);
    return m_numberOfOperands - 1;
  }

 private:
  struct OperatorString {
    OperatorType type;
    const char* mainString;
    const char* alternativeString;
  };

  constexpr static OperatorString k_operatorStrings[] = {
      {OperatorType::Equal, "=", nullptr},
      {OperatorType::NotEqual, "≠", "!="},  // NFKD norm on "≠"
      {OperatorType::Superior, ">", nullptr},
      {OperatorType::Inferior, "<", nullptr},
      {OperatorType::SuperiorEqual, "≥", ">="},
      {OperatorType::InferiorEqual, "≤", "<="}};
  static_assert(std::size(k_operatorStrings) ==
                    static_cast<int>(OperatorType::NumberOfTypes),
                "Missing string for comparison operator.");

  /* This constructor takes its last operator separately so that you can
   * copy another ComparisonNode and add an operator at the end of it. */
  ComparisonNode(int numberOfOperands, OperatorType* operatorsListButTheLast,
                 OperatorType lastOperatorOfList);
  ComparisonNode(OperatorType operatorType)
      : ComparisonNode(2, nullptr, operatorType) {}

  // Layout
  LayoutShape leftLayoutShape() const override { return LayoutShape::Default; }
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  bool childNeedsSystemParenthesesAtSerialization(
      const PoolObject* child) const override;

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  // Simplification
  OExpression shallowReduce(const ReductionContext& ReductionContext) override;

  uint16_t m_numberOfOperands;
  /* This variable contains the list of operators to insert between the
   * operands. Example: in "a=b<c", the operators list is {=,<}. */
  OperatorType m_operatorsList[0];  // Must be last
};

class Comparison : public OExpression {
 public:
  Comparison(const ComparisonNode* n) : OExpression(n) {}
  static Comparison Builder(OExpression child0,
                            ComparisonNode::OperatorType operatorType,
                            OExpression child1);
  Comparison addComparison(ComparisonNode::OperatorType operatorType,
                           OExpression child);

  OExpression shallowReduce(ReductionContext ReductionContext);
  ComparisonNode::OperatorType operatorAtIndex(int i) const {
    return node()->operatorAtIndex(i);
  }
  /* Returns Undefined if contains an equal. */
  OExpression cloneWithStrictOperators() const {
    return cloneWithStrictOrLenientOperators(true);
  }
  /* Changes not equal into equal. */
  OExpression cloneWithLenientOperators() const {
    return cloneWithStrictOrLenientOperators(false);
  }
  int numberOfOperators() const { return node()->numberOfOperators(); }

 private:
  OExpression cloneWithStrictOrLenientOperators(bool strict) const;
  ComparisonNode* node() const {
    return static_cast<ComparisonNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
