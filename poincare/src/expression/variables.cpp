#include "variables.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>
#include <poincare/symbol_context.h>
#include <string.h>

#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "symbol.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

Variables::Variable::Variable(uint8_t id, ComplexProperties properties) {
  Tree* temp = SharedTreeStack->pushVar(id, properties);
  assert(k_size == temp->treeSize());
  temp->copyTreeTo(m_blocks);
  temp->removeTree();
}

uint8_t Variables::Id(const Tree* variable) {
  assert(variable->isVar());
  return variable->nodeValue(0);
}

ComplexProperties Variables::GetComplexProperties(const Tree* variable) {
  assert(variable->isVar());
  return ComplexProperties::FromValue(variable->nodeValue(1),
                                      variable->nodeValue(2));
}

uint8_t Variables::Private::ToId(const Tree* variables, const char* name,
                                 uint8_t length) {
  for (IndexedChild<const Tree*> child : variables->indexedChildren()) {
    if (Symbol::IsNamed(child, name)) {
      return child.index;
    }
  }
  OMG::unreachable();  // Not found
}

const Tree* Variables::Private::ToSymbol(const Tree* variables, uint8_t id) {
  return variables->child(id);
}

Tree* Variables::GetUserSymbols(const Tree* e,
                                const Poincare::SymbolContext& symbolContext) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  Private::GetUserSymbols(e, set, symbolContext);
  return set;
}

void Variables::Private::GetUserSymbols(
    const Tree* e, Tree* set, const Poincare::SymbolContext& symbolContext) {
  if (e->isUserSymbol()) {
    return Set::Add(set, e);
  }
  if (e->isUserFunction()) {
    assert(symbolContext.expressionForUserNamed(e));
    /* If symbolContext is given, we look inside the user function definition.
     * Unknown symbol has to be discarded. */
    Tree* subSet = Set::PushEmpty();
    GetUserSymbols(symbolContext.expressionForUserNamed(e), subSet,
                   symbolContext);
    Tree* symbolToRemove = KList(KUnknownSymbol)->cloneTree();
    subSet = Set::Difference(subSet, symbolToRemove);
    set = Set::Union(set, subSet);
    // Also get UserSymbols in the function's child.
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
    } else if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      Tree* subSet = Set::PushEmpty();
      GetUserSymbols(child, subSet, symbolContext);
      Tree* boundSymbols = Set::PushEmpty();
      Set::Add(boundSymbols, e->child(Parametric::k_variableIndex));
      subSet = Set::Difference(subSet, boundSymbols);
      set = Set::Union(set, subSet);
    } else {
      GetUserSymbols(child, set, symbolContext);
    }
  }
}

bool Variables::HasUserSymbols(const Tree* e, bool checkForUserFunctions) {
  if (e->isUserSymbol() || (checkForUserFunctions && e->isUserFunction())) {
    return true;
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
      continue;
    }
    if (HasUserSymbols(child, checkForUserFunctions)) {
      return true;
    }
  }
  return false;
}

bool Variables::LeaveScopeWithReplacement(Tree* e, const Tree* value,
                                          bool reduce,
                                          bool addDependencyInValue) {
  bool changed = Replace(e, 0, value, true, reduce);
  if (!changed && addDependencyInValue) {
    /* Add a dependency in the value if it is leaving the scope of the
     * expression but it does not appear in the expression. */
    /* TODO We need to track the replacement value only if it is in the pool and
     * after e. Cloning is overkill but easy. */
    e->moveTreeOverTree(PatternMatching::Create(KDep(KA, KDepList(KB)),
                                                {.KA = e, .KB = value}));
    if (reduce) {
      SystematicReduction::ShallowReduce(e);
    }
    return true;
  }
  return changed;
}

bool Variables::Replace(Tree* e, const Tree* variable, const Tree* value,
                        bool reduce) {
  assert(variable->isVar());
  return Replace(e, Id(variable), value, false, reduce);
}

bool Variables::Replace(Tree* e, int id, const Tree* value, bool leave,
                        bool reduce) {
  /* TODO We need to track the replacement value only if it is in the pool and
   * after e. Cloning is overkill but easy. */
  TreeRef valueRef = value->cloneTree();
  bool result = Replace(e, id, valueRef, leave, reduce);
  valueRef->removeTree();
  return result;
}

bool Variables::Replace(Tree* e, int id, const TreeRef& value, bool leave,
                        bool reduce) {
  if (e->isVar()) {
    if (Id(e) == id) {
      e->cloneTreeOverTree(value);
      return true;
    }
    if (leave && Id(e) > id) {
      e->setNodeValue(0, Id(e) - 1);
      return true;
    }
    return false;
  }
  bool isParametric = e->isParametric();
  bool changed = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    int updatedId =
        id + (isParametric && Parametric::IsFunctionIndex(child.index, e));
    changed = Replace(child, updatedId, value, leave,
                      /* Leave dependencies unreduced in the systematic
                       * reduction (they are handled separately). */
                      child->isDepList() ? false : reduce) ||
              changed;
  }
  if (reduce && changed) {
    SystematicReduction::ShallowReduce(e);
  }
  return changed;
}

bool Variables::ReplaceSymbol(Tree* e, const Tree* symbol, int id,
                              ComplexProperties properties) {
  assert(symbol->isUserSymbol());
  return ReplaceSymbol(e, Symbol::GetName(symbol), id, properties);
}

bool Variables::ReplaceSymbol(Tree* e, const char* symbol, int id,
                              ComplexProperties properties) {
  if (Symbol::IsUserSymbol(e, symbol)) {
    Tree* var = SharedTreeStack->pushVar(static_cast<uint8_t>(id), properties);
    e->moveTreeOverTree(var);
    return true;
  }
  bool isParametric = e->isParametric();
  bool changed = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
    } else if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      // No need to continue if symbol is hidden by a local definition
      if (!Symbol::IsUserSymbol(e->child(Parametric::k_variableIndex),
                                symbol)) {
        changed = ReplaceSymbol(child, symbol, id + 1, properties) || changed;
      }
    } else {
      changed = ReplaceSymbol(child, symbol, id, properties) || changed;
    }
  }
  return changed;
}

void Variables::ReplaceUserFunctionOrSequenceWithTree(Tree* e,
                                                      const Tree* replacement) {
  assert(e->isUserFunction() || e->isUserSequence());
  // Otherwise, local variable scope should be handled.
  assert(!Variables::HasVariables(replacement));
  TreeRef evaluateAt = e->child(0)->cloneTree();
  e->cloneTreeOverTree(replacement);
  if (!e->deepReplaceWith(KUnknownSymbol, evaluateAt)) {
    // If f(x) does not depend on x, add a dependency on x
    e->moveTreeOverTree(PatternMatching::Create(KDep(KA, KDepList(KB)),
                                                {.KA = e, .KB = evaluateAt}));
  }
  evaluateAt->removeTree();
}

/* TODO: This could be factorized with other methods, such as Replace,
 * ReplaceSymbol or Projection::DeepReplaceUserNamed. */
bool Variables::ReplaceSymbolWithTree(Tree* e, const Tree* symbol,
                                      const Tree* replacement) {
  bool isParametric = e->isParametric();
  bool changed = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    /* Do not replace parametric's variable and symbols hidden by a local
     * definition */
    if (!(isParametric &&
          (child.index == Parametric::k_variableIndex ||
           (Parametric::IsFunctionIndex(child.index, e) &&
            Symbol::IsNamed(e->child(Parametric::k_variableIndex),
                            Symbol::GetName(symbol)))))) {
      changed = ReplaceSymbolWithTree(child, symbol, replacement) || changed;
    }
  }
  if (symbol->isUserNamed() && symbol->nodeIsIdenticalTo(e)) {
    if (symbol->isUserSymbol() || symbol->treeIsIdenticalTo(e)) {
      e->cloneTreeOverTree(replacement);
      return true;
    }
    if (symbol->child(0)->isUserSymbol()) {
      ReplaceUserFunctionOrSequenceWithTree(e, replacement);
      return true;
    }
    return false;
  }
  return changed;
}

bool Variables::ProjectLocalVariablesToId(Tree* e, uint8_t depth) {
  bool changed = false;
  bool isParametric = e->isParametric();
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
    } else if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      // Project local variable
      changed = ReplaceSymbol(child, e->child(Parametric::k_variableIndex), 0,
                              Parametric::VariableProperties(e)) ||
                changed;
      changed = ProjectLocalVariablesToId(child, depth + 1) || changed;
    } else {
      changed = ProjectLocalVariablesToId(child, depth) || changed;
    }
  }
  return changed;
}

bool Variables::BeautifyToName(Tree* e, uint8_t depth) {
  assert(!e->isVar());
  bool changed = false;
  bool isParametric = e->isParametric();
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      // beautify variable introduced by this scope
      // TODO: check that name is available here or make new name
      changed = Replace(child, 0, e->child(Parametric::k_variableIndex), false,
                        false) ||
                changed;
      // beautify outer variables
      changed = BeautifyToName(child, depth + 1) || changed;
      continue;
    }
    changed = BeautifyToName(child, depth) || changed;
  }
  return changed;
}

static bool HasVariablesOutOfScope(const Tree* e, int scope) {
  if (e->isVar()) {
    return Variables::Id(e) >= scope;
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    int childScope =
        scope + (isParametric && Parametric::IsFunctionIndex(child.index, e));
    if (HasVariablesOutOfScope(child, childScope)) {
      return true;
    }
  }
  return false;
}

bool Variables::IsVariableWithId(const Tree* e, int id) {
  return e->isVar() && Id(e) == id;
}

bool Variables::HasVariables(const Tree* e) {
  return HasVariablesOutOfScope(e, 0);
}

bool Variables::HasVariable(const Tree* e, const Tree* variable) {
  // TODO: variable must have the same scope as e
  assert(variable->isVar());
  return HasVariable(e, Id(variable));
}

bool Variables::HasVariable(const Tree* e, int id) {
  if (IsVariableWithId(e, id)) {
    return true;
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    int updatedId =
        id + (isParametric && Parametric::IsFunctionIndex(child.index, e));
    if (HasVariable(child, updatedId)) {
      return true;
    }
  }
  return false;
}

void Variables::Private::EnterOrLeaveScope(Tree* e, bool enter, int var) {
  assert(!e->isRandomized());
  if (e->isVar()) {
    uint8_t id = Id(e);
    if (id > var) {
      assert(enter || id > 0);
      e->setNodeValue(0, enter ? id + 1 : id - 1);
    }
    return;
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    int updatedId =
        var + (isParametric && Parametric::IsFunctionIndex(child.index, e));
    EnterOrLeaveScope(child, enter, updatedId);
  }
}

uint8_t Variables::GetScopeOf(const Tree* tree, const Tree* root) {
  assert(root <= tree && tree < root->nextTree());
  uint8_t scope = 0;
  // From [root] to [tree], visit each ancestors of [tree].
  while (root != tree) {
    /* If [root] is parametric and [tree] is located in the functionIndex
     * child, scope will increase */
    int functionIndex =
        root->isParametric() ? Parametric::FunctionIndex(root) : -1;
    const Tree* child = root->child(0);
    const Tree* nextChild = child->nextTree();
    // Find the ancestor of [tree] in [root]'s children
    for (int i = 0; i < root->numberOfChildren(); ++i) {
      // if [tree] is a child of [child] (or [tree] is [child])
      if (child <= tree && tree < nextChild) {
        if (functionIndex == i) {
          ++scope;
        }
        root = child;
        break;
      }
      child = nextChild;
      nextChild = child->nextTree();
    }
  }
  return scope;
}

}  // namespace Poincare::Internal
