#ifndef POINCARE_EXPRESSION_BINARY_H
#define POINCARE_EXPRESSION_BINARY_H

#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Binary {
 public:
  static bool IsBinaryLogicalOperator(LayoutSpan name, Type* type);
  static const char* OperatorName(TypeBlock type);

  static bool IsComparisonOperatorString(LayoutSpan name, Type* returnType,
                                         size_t* returnLength);

  static const char* ComparisonOperatorName(TypeBlock type);

  static bool SimplifyBooleanOperator(Tree* tree);
  TREE_REF_WRAP(SimplifyBooleanOperator);

  static bool SimplifyComparison(Tree* tree);
  TREE_REF_WRAP(SimplifyComparison);

  static bool SimplifyPiecewise(Tree* tree);
  TREE_REF_WRAP(SimplifyPiecewise);

 private:
  constexpr static const char* k_logicalNotName = "not";
  struct TypeAndName {
    Type type;
    const char* name;
  };
  constexpr static int k_numberOfOperators = 5;
  constexpr static TypeAndName k_operatorNames[] = {{Type::LogicalAnd, "and"},
                                                    {Type::LogicalOr, "or"},
                                                    {Type::LogicalXor, "xor"},
                                                    {Type::LogicalNand, "nand"},
                                                    {Type::LogicalNor, "nor"}};
  static_assert(std::size(k_operatorNames) == k_numberOfOperators,
                "Wrong number of binary logical operators");

  struct OperatorString {
    Type type;
    const char* mainString;
    const char* alternativeString;
  };

  constexpr static int k_numberOfComparisons = 6;
  constexpr static OperatorString k_operatorStrings[] = {
      {Type::Equal, "=", nullptr},
      {Type::NotEqual, "≠", "!="},  // NFKD norm on "≠"
      {Type::Superior, ">", nullptr},
      {Type::Inferior, "<", nullptr},
      {Type::SuperiorEqual, "≥", ">="},
      {Type::InferiorEqual, "≤", "<="}};
  static_assert(std::size(k_operatorStrings) == k_numberOfComparisons,
                "Missing string for comparison operator.");
};

}  // namespace Poincare::Internal

#endif
