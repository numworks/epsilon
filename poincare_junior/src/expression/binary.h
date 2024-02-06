#ifndef POINCARE_EXPRESSION_BINARY_H
#define POINCARE_EXPRESSION_BINARY_H

#include <poincare_junior/src/layout/rack_layout_decoder.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Binary {
 public:
  static bool IsBinaryLogicalOperator(const CPL* name, int nameLength,
                                      BlockType* type);

  constexpr static const char* k_logicalNotName = "not";

 private:
  constexpr static int k_numberOfOperators = 5;
  struct OperatorName {
    BlockType type;
    const char* name;
  };
  constexpr static OperatorName k_operatorNames[] = {
      {BlockType::LogicalAnd, "and"},
      {BlockType::LogicalOr, "or"},
      {BlockType::LogicalXor, "xor"},
      {BlockType::LogicalNand, "nand"},
      {BlockType::LogicalNor, "nor"}};
  static_assert(std::size(k_operatorNames) == k_numberOfOperators,
                "Wrong number of binary logical operators");
};

}  // namespace PoincareJ

#endif
