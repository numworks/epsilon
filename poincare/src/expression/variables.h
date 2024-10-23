#ifndef POINCARE_EXPRESSION_VARIABLES_H
#define POINCARE_EXPRESSION_VARIABLES_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

/* Textual UserSymbols in expressions are projected into de Bruijn indices. When
 * a scope (parametric) is entered, all the indices are shifted by one which
 * leaves room to represent the new local variable with the index \0. The user
 * symbol of the local variable is kept as an hint for the beautification.
 *
 * For instance:  x + sum(x + 2k, k, 0, n) => \0 + sum(\1 + 2*\0, k, 0, \1)
 *
 * Variable are also given a complex sign restrained by the context. */

class Variables {
 public:
  // Used to store a Variable constant tree on the stack.
  class Variable {
   public:
    Variable(uint8_t id, ComplexSign sign);
    operator const Tree*() const { return Tree::FromBlocks(m_blocks); }

   private:
    constexpr static size_t k_size = TypeBlock::NumberOfMetaBlocks(Type::Var);
    Block m_blocks[k_size];
  };
  // Push a Set with the free user symbols of the expression
  static Tree* GetUserSymbols(const Tree* e);
  static bool HasUserSymbols(const Tree* e);

  static bool ProjectLocalVariablesToId(Tree* e, uint8_t depth = 0);
  static bool BeautifyToName(Tree* e, uint8_t depth = 0);
  static uint8_t Id(const Tree* variable);
  static ComplexSign GetComplexSign(const Tree* variable);

  // On projected expressions
  static bool HasVariables(const Tree* e);

  // On projected expressions
  static bool HasVariable(const Tree* e, const Tree* variable);
  static bool HasVariable(const Tree* e, int id);

  // Replace occurrences of variable with value and simplify inside e
  static bool Replace(Tree* e, const Tree* variable, const Tree* value,
                      bool simplify);
  static bool Replace(Tree* e, int id, const Tree* value, bool leave = false,
                      bool simplify = true);
  static bool Replace(Tree* e, int id, const TreeRef& value, bool leave = false,
                      bool simplify = true);

  // Replace symbol with Var<id>
  static bool ReplaceSymbol(Tree* e, const char* symbol, int id,
                            ComplexSign sign);
  static bool ReplaceSymbol(Tree* e, const Tree* symbol, int id,
                            ComplexSign sign);
  static void ReplaceUserFunctionOrSequenceWithTree(Tree* e,
                                                    const Tree* replacement);
  static bool ReplaceSymbolWithTree(Tree* e, const Tree* symbol,
                                    const Tree* replacement);

  // Increment all variable indexes
  static void EnterScope(Tree* e) { return EnterOrLeaveScope(e, true, -1); }
  // Increment non null variable indexes
  static void EnterScopeExceptLocalVariable(Tree* e) {
    return EnterOrLeaveScope(e, true, 0);
  }
  // Decrement variables indexes
  static void LeaveScope(Tree* e) { return EnterOrLeaveScope(e, false, 0); }
  static bool LeaveScopeWithReplacement(Tree* e, const Tree* value,
                                        bool simplify) {
    return Replace(e, 0, value, true, simplify);
  }
  TREE_REF_WRAP_2(LeaveScopeWithReplacement, const Tree*, bool);

 private:
  // Variables below preserveUnder are considered local and preserved
  static void EnterOrLeaveScope(Tree* e, bool enter, int preserveUnder);
  static void GetUserSymbols(const Tree* e, Tree* set);
  static uint8_t ToId(const Tree* variables, const char* name, uint8_t length);
  static const Tree* ToSymbol(const Tree* variables, uint8_t id);
};

}  // namespace Poincare::Internal
#endif
