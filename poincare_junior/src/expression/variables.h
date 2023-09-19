#ifndef POINCARE_EXPRESSION_VARIABLES_H
#define POINCARE_EXPRESSION_VARIABLES_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Variables {
 public:
  // Used to store a Variable constant tree on the stack.
  class Variable {
   public:
    Variable(uint8_t id);
    operator const Tree*() const { return Tree::FromBlocks(m_blocks); }

   private:
    constexpr static size_t k_size =
        TypeBlock::NumberOfMetaBlocks(BlockType::Variable);
    Block m_blocks[k_size];
  };
  // Push a Set with the variables of the expression
  static Tree* GetVariables(const Tree* t);
  static void ProjectToId(Tree* t, const Tree* variables);
  static void BeautifyToName(Tree* t, const Tree* variables);
  static uint8_t Id(const Tree* variable);

 private:
  static uint8_t ToId(const Tree* variables, const char* name, uint8_t length);
  static const Tree* ToSymbol(const Tree* variables, uint8_t id);
};

}  // namespace PoincareJ
#endif
