#ifndef POINCARE_EXPRESSION_BINARY_H
#define POINCARE_EXPRESSION_BINARY_H

#include <poincare/comparison_operator.h>
#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Binary {
 public:
  static bool IsBinaryLogicalOperator(LayoutSpan name, Type* type);
  static const char* OperatorName(TypeBlock type);

  static bool IsComparisonOperatorString(LayoutSpan name, Type* returnType,
                                         size_t* returnLength);
  static ComparisonJunior::Operator ComparisonOperatorForType(TypeBlock type);

  static bool ReduceBooleanOperator(Tree* e);

  static bool ReduceComparison(Tree* e);

  static bool ReducePiecewise(Tree* e);

 private:
  constexpr static const char* k_logicalNotName = "not";
  struct TypeAndName {
    Type type;
    const char* name;
  };
#if POINCARE_BOOLEAN
  constexpr static int k_numberOfOperators = 5;
  constexpr static TypeAndName k_operatorNames[] = {{Type::LogicalAnd, "and"},
                                                    {Type::LogicalOr, "or"},
                                                    {Type::LogicalXor, "xor"},
                                                    {Type::LogicalNand, "nand"},
                                                    {Type::LogicalNor, "nor"}};
  static_assert(std::size(k_operatorNames) == k_numberOfOperators,
                "Wrong number of binary logical operators");
#else
  constexpr static int k_numberOfOperators = 0;
  constexpr static TypeAndName k_operatorNames[0] = {};
#endif

  struct OperatorForType {
    Type type;
    ComparisonJunior::Operator op;
  };

#if POINCARE_BOOLEAN
  constexpr static int k_numberOfComparisons = 6;
  constexpr static OperatorForType k_operatorForType[] = {
      {Type::Equal, ComparisonJunior::Operator::Equal},
      {Type::NotEqual, ComparisonJunior::Operator::NotEqual},
      {Type::Superior, ComparisonJunior::Operator::Superior},
      {Type::Inferior, ComparisonJunior::Operator::Inferior},
      {Type::SuperiorEqual, ComparisonJunior::Operator::SuperiorEqual},
      {Type::InferiorEqual, ComparisonJunior::Operator::InferiorEqual},
  };
  static_assert(std::size(k_operatorForType) == k_numberOfComparisons,
                "Missing comparison  operator for type.");
#else
  constexpr static int k_numberOfComparisons = 0;
  constexpr static OperatorForType k_operatorForType[0] = {};
#endif
};

}  // namespace Poincare::Internal

#endif
