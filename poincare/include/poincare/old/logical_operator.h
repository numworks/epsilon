#ifndef POINCARE_LOGICAL_OPERATOR_H
#define POINCARE_LOGICAL_OPERATOR_H

#include <array>

#include "old_expression.h"

namespace Poincare {

class LogicalOperatorNode : public ExpressionNode {
 public:
  /* not: 2
   * and, nand: 1
   * or, xor, nor: 0
   * -1 if not a logical operator type. */
  static int LogicalOperatorTypePrecedence(
      const ExpressionNode* operatorExpression);

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "LogicalOperator";
  }
  void logAttributes(std::ostream& stream) const override {
    stream << " operator=\"" << operatorName();
  }
#endif
 protected:
  constexpr static int k_sizeOfNameBuffer = 1 + 4 + 1 + 1;
  //                                       " "+"nand"+" "+nullTermination
  virtual const char* operatorName() const = 0;

 private:
  LayoutShape leftLayoutShape() const override { return LayoutShape::Default; }
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;
};

// Not Operator

class LogicalOperatorNotNode : public LogicalOperatorNode {
 public:
  constexpr static const char* k_name = "not";
  size_t size() const override { return sizeof(LogicalOperatorNotNode); }
  Type otype() const override { return Type::LogicalOperatorNot; }
  int numberOfChildren() const override { return 1; }

 private:
  const char* operatorName() const override { return k_name; }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Reduction
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
};

class LogicalOperatorNot
    : public ExpressionOneChild<LogicalOperatorNot, LogicalOperatorNotNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

// Binary Logical Operator

class BinaryLogicalOperatorNode : public LogicalOperatorNode {
 public:
  enum class OperatorType : uint8_t {
    And,
    Or,
    Xor,
    Nand,
    Nor,
    LengthOfEnum  // Used to compute length of enum.
  };
  static bool IsBinaryLogicalOperator(const char* name, int nameLength,
                                      OperatorType* type);

  Type otype() const override { return Type::BinaryLogicalOperator; }
  size_t size() const override { return sizeof(BinaryLogicalOperatorNode); }
  int numberOfChildren() const override { return 2; }

  OperatorType operatorType() const { return m_typeOfOperator; }
  void setOperatorType(OperatorType type) { m_typeOfOperator = type; }
  bool evaluate(bool a, bool b) const;

 private:
  constexpr static int k_numberOfOperators =
      static_cast<int>(OperatorType::LengthOfEnum);
  struct OperatorName {
    OperatorType type;
    const char* name;
  };
  constexpr static OperatorName k_operatorNames[] = {
      {OperatorType::And, "and"},
      {OperatorType::Or, "or"},
      {OperatorType::Xor, "xor"},
      {OperatorType::Nand, "nand"},
      {OperatorType::Nor, "nor"}};
  static_assert(std::size(k_operatorNames) == k_numberOfOperators,
                "Wrong number of binary logical operators");

  const char* operatorName() const override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  // Reduction
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  OperatorType m_typeOfOperator;
};

class BinaryLogicalOperator : public OExpression {
 public:
  BinaryLogicalOperator(const BinaryLogicalOperatorNode* n) : OExpression(n) {}
  static BinaryLogicalOperator Builder(
      OExpression firstChild, OExpression secondChild,
      BinaryLogicalOperatorNode::OperatorType type);
  OExpression shallowReduce(ReductionContext reductionContext);
  BinaryLogicalOperatorNode::OperatorType operatorType() const {
    return node()->operatorType();
  }

 private:
  BinaryLogicalOperatorNode* node() const {
    return static_cast<BinaryLogicalOperatorNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
