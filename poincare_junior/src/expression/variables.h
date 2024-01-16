#ifndef POINCARE_EXPRESSION_VARIABLES_H
#define POINCARE_EXPRESSION_VARIABLES_H

#include <poincare_junior/src/memory/tree.h>

#include "sign.h"

namespace PoincareJ {

/* Textual UserSymbols in expressions are projected into de Bruijn indices.  The
 * global free variables have an index corresponding to their alphabetical order
 * in global variables.  When a scope (parametric) is entered, all the indices
 * are shifted by one which leaves room to represent the new local variable with
 * the index \0. The user symbol of the local variable is kept as an hint for
 * the beautification.
 * For instance:  x + sum(x + 2k, k, 0, n) => \0 + sum(\1 + 2*\0, k, 0, \1)
 *
 * Variable are also given a context restrained by the context. */

class Variables {
 public:
  // Used to store a Variable constant tree on the stack.
  class Variable {
   public:
    Variable(uint8_t id, Sign::ComplexSign sign);
    operator const Tree*() const { return Tree::FromBlocks(m_blocks); }

   private:
    constexpr static size_t k_size =
        TypeBlock::NumberOfMetaBlocks(BlockType::Variable);
    Block m_blocks[k_size];
  };
  // Push a Set with the free user symbols of the expression
  static Tree* GetUserSymbols(const Tree* t);
  static void ProjectToId(Tree* t, const Tree* variables,
                          Sign::ComplexSign sign, uint8_t depth = 0);
  static void BeautifyToName(Tree* t, const Tree* variables, uint8_t depth = 0);
  static uint8_t Id(const Tree* variable);
  static Sign::ComplexSign GetComplexSign(const Tree* variable);

  // On projected expressions
  static bool HasVariables(const Tree* t);

  // On projected expressions
  static bool HasVariable(const Tree* t, const Tree* variable);
  static bool HasVariable(const Tree* t, int id);

  // Replace occurrences of variable with value and simplify inside expr
  static bool Replace(Tree* expr, const Tree* variable, const Tree* value);
  static bool Replace(Tree* expr, int id, const Tree* value,
                      bool leave = false);

  // Increment variables indexes
  static void EnterScope(Tree* expr);
  // Decrement variables indexes
  static void LeaveScope(Tree* expr);
  static void LeaveScopeWithReplacement(Tree* expr, const Tree* value) {
    Replace(expr, 0, value, true);
  }

 private:
  static void GetUserSymbols(const Tree* t, Tree* set);
  static bool ReplaceSymbol(Tree* expr, const Tree* symbol, int id,
                            Sign::ComplexSign sign);
  static uint8_t ToId(const Tree* variables, const char* name, uint8_t length);
  static const Tree* ToSymbol(const Tree* variables, uint8_t id);
};

}  // namespace PoincareJ
#endif
