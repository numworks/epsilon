#pragma once

#include <poincare/comparison_operator.h>
#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Binary {
 public:
  static bool IsBinaryLogicalOperator(LayoutSpan name, Type* type);
  static const char* OperatorName(TypeBlock type);

  /*  REFACTOR: return the returnType and returnLength when the result is an
   * comparison operator. This would avoid creating uninitialized returnType and
   * returnLength before passing them to this function. The return type could be
   * an std::optional<std::pair<Type, size_t>>, that contains the {Type, size_t}
   * result only when there is a comparison operator. */
  static bool IsComparisonOperatorString(LayoutSpan name, Type* returnType,
                                         size_t* returnLength);
  static Comparison::Operator ComparisonOperatorForType(TypeBlock type);

  static bool ReduceBooleanOperator(Tree* e);

  static bool ReduceComparison(Tree* e);

  static bool ReducePiecewise(Tree* e);
#if POINCARE_PIECEWISE
  /* Return the derivative of a piecewise derivand. Return nullptr if unhandled.
   * If all conditions are handled, distribute derivation on branches, and add
   * undef branches at each condition breakpoints. For example:
   * piecewise(f1(x),  x>1,
   *           f2(x),  x≤2,
   *           f3(x),  x=3,
   *           f4(x),  x≠4,
   *           f5(x))
   * Is derived to
   * piecewise(f'1(x), x>1,   undef,  x≥1,
   *           f'2(x), x<2,   undef,  x≤2,
   *           f'3(x), false, undef,  x=3,
   *           f'4(x), x≠4,   undef,  true,
   *           f'5(x))
   * And then reduced.
   */
  static Tree* ReducePiecewiseDerivative(const Tree* symbol,
                                         const Tree* symbolValue,
                                         const Tree* order,
                                         const Tree* derivand);
#endif

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

#if POINCARE_PIECEWISE
  /* Make reduced piecewise condition [e] lenient.
   * Return false if unhandled. Examples :
   * a > b  is turned into a >= b
   * a <= b is turned into a <= b
   * a != b is turned into True
   * xor(a > b, a >= c) returns false. */
  static bool MakeLenient(Tree* e);
  /* Make reduced piecewise condition [e] strict.
   * Return false if unhandled. Examples :
   * a >= b is turned into a > b
   * a < b  is turned into a < b
   * a = b  is turned into False
   * xor(a > b, a >= c) returns false. */
  static bool MakeStrict(Tree* e);
#endif

  struct OperatorForType {
    Type type;
    Comparison::Operator op;
  };

#if POINCARE_BOOLEAN
  constexpr static int k_numberOfComparisons = 6;
  constexpr static OperatorForType k_operatorForType[] = {
      {Type::Equal, Comparison::Operator::Equal},
      {Type::NotEqual, Comparison::Operator::NotEqual},
      {Type::Superior, Comparison::Operator::Superior},
      {Type::Inferior, Comparison::Operator::Inferior},
      {Type::SuperiorEqual, Comparison::Operator::SuperiorEqual},
      {Type::InferiorEqual, Comparison::Operator::InferiorEqual},
  };
  static_assert(std::size(k_operatorForType) == k_numberOfComparisons,
                "Missing comparison  operator for type.");
#else
  constexpr static int k_numberOfComparisons = 0;
  constexpr static OperatorForType k_operatorForType[0] = {};
#endif
};

}  // namespace Poincare::Internal
