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

namespace Variables {
// Used to store a Variable constant tree on the stack.
class Variable {
 public:
  Variable(uint8_t id, ComplexSign sign);
  operator const Tree*() const { return Tree::FromBlocks(m_blocks); }

 private:
  constexpr static size_t k_size = TypeBlock::NumberOfMetaBlocks(Type::Var);
  Block m_blocks[k_size];
};

namespace Private {
// Variables below preserveUnder are considered local and preserved
void EnterOrLeaveScope(Tree* e, bool enter, int preserveUnder);
void GetUserSymbols(const Tree* e, Tree* set);
uint8_t ToId(const Tree* variables, const char* name, uint8_t length);
const Tree* ToSymbol(const Tree* variables, uint8_t id);
}  // namespace Private

// Push a Set with the free user symbols of the expression
Tree* GetUserSymbols(const Tree* e);
// Ignore UserSymbols in parametric's variable
bool HasUserSymbols(const Tree* e);

bool ProjectLocalVariablesToId(Tree* e, uint8_t depth = 0);
bool BeautifyToName(Tree* e, uint8_t depth = 0);
uint8_t Id(const Tree* variable);
ComplexSign GetComplexSign(const Tree* variable);

// On projected expressions
bool HasVariables(const Tree* e);

// On projected expressions
bool HasVariable(const Tree* e, const Tree* variable);
bool HasVariable(const Tree* e, int id);

// Replace occurrences of variable with value and simplify inside e
bool Replace(Tree* e, const Tree* variable, const Tree* value, bool simplify);
bool Replace(Tree* e, int id, const Tree* value, bool leave = false,
             bool simplify = true);
bool Replace(Tree* e, int id, const TreeRef& value, bool leave = false,
             bool simplify = true);

// Replace symbol with Var<id>
bool ReplaceSymbol(Tree* e, const char* symbol, int id, ComplexSign sign);
bool ReplaceSymbol(Tree* e, const Tree* symbol, int id, ComplexSign sign);
void ReplaceUserFunctionOrSequenceWithTree(Tree* e, const Tree* replacement);
bool ReplaceSymbolWithTree(Tree* e, const Tree* symbol,
                           const Tree* replacement);

// Increment all variable indexes
inline void EnterScope(Tree* e) {
  return Private::EnterOrLeaveScope(e, true, -1);
}
// Increment non null variable indexes
inline void EnterScopeExceptLocalVariable(Tree* e) {
  return Private::EnterOrLeaveScope(e, true, 0);
}
// Decrement variables indexes
inline void LeaveScope(Tree* e) {
  return Private::EnterOrLeaveScope(e, false, 0);
}
bool LeaveScopeWithReplacement(Tree* e, const Tree* value, bool simplify,
                               bool addDependencyInValue);

};  // namespace Variables

}  // namespace Poincare::Internal
#endif
