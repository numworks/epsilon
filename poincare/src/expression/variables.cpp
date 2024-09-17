#include "variables.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>
#include <string.h>

#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "symbol.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

Variables::Variable::Variable(uint8_t id, ComplexSign sign) {
  Tree* temp = SharedTreeStack->pushVar(id, sign);
  assert(k_size == temp->treeSize());
  temp->copyTreeTo(m_blocks);
  temp->removeTree();
}

uint8_t Variables::Id(const Tree* variable) {
  assert(variable->isVar());
  return variable->nodeValue(0);
}

ComplexSign Variables::GetComplexSign(const Tree* variable) {
  assert(variable->isVar());
  return ComplexSign::FromValue(variable->nodeValue(1), variable->nodeValue(2));
}

uint8_t Variables::ToId(const Tree* variables, const char* name,
                        uint8_t length) {
  for (IndexedChild<const Tree*> child : variables->indexedChildren()) {
    if (strcmp(Symbol::GetName(child), name) == 0) {
      return child.index;
    }
  }
  OMG::unreachable();  // Not found
}

const Tree* Variables::ToSymbol(const Tree* variables, uint8_t id) {
  return variables->child(id);
}

Tree* Variables::GetUserSymbols(const Tree* e) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  GetUserSymbols(e, set);
  return set;
}

void Variables::GetUserSymbols(const Tree* e, Tree* set) {
  if (e->isUserSymbol()) {
    return Set::Add(set, e);
  }
  bool isParametric = e->isParametric();
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
    } else if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      Tree* subSet = Set::PushEmpty();
      GetUserSymbols(child, subSet);
      Tree* boundSymbols = Set::PushEmpty();
      Set::Add(boundSymbols, e->child(Parametric::k_variableIndex));
      subSet = Set::Difference(subSet, boundSymbols);
      set = Set::Union(set, subSet);
    } else {
      GetUserSymbols(child, set);
    }
  }
}

bool Variables::HasUserSymbols(const Tree* e) {
  return e->hasDescendantSatisfying(
      [](const Tree* e) { return e->isUserSymbol(); });
}

bool Variables::Replace(Tree* e, const Tree* variable, const Tree* value,
                        bool simplify) {
  assert(variable->isVar());
  return Replace(e, Id(variable), value, simplify);
}

bool Variables::Replace(Tree* e, int id, const Tree* value, bool leave,
                        bool simplify) {
  /* TODO We need to track the replacement value only if it is in the pool and
   * after e. Cloning is overkill but easy. */
  TreeRef valueRef = value->cloneTree();
  bool result = Replace(e, id, valueRef, leave, simplify);
  valueRef->removeTree();
  return result;
}

bool Variables::Replace(Tree* e, int id, const TreeRef& value, bool leave,
                        bool simplify) {
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
    changed = Replace(child, updatedId, value, leave, simplify) || changed;
  }
  if (simplify && changed) {
    SystematicReduction::ShallowReduce(e);
  }
  return changed;
}

bool Variables::ReplaceSymbol(Tree* e, const Tree* symbol, int id,
                              ComplexSign sign) {
  assert(symbol->isUserSymbol());
  return ReplaceSymbol(e, Symbol::GetName(symbol), id, sign);
}

bool Variables::ReplaceSymbol(Tree* e, const char* symbol, int id,
                              ComplexSign sign) {
  if (e->isUserSymbol() && strcmp(Symbol::GetName(e), symbol) == 0) {
    Tree* var = SharedTreeStack->pushVar(static_cast<uint8_t>(id), sign);
    e->moveTreeOverTree(var);
    return true;
  }
  bool isParametric = e->isParametric();
  bool changed = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (isParametric && child.index == Parametric::k_variableIndex) {
    } else if (isParametric && Parametric::IsFunctionIndex(child.index, e)) {
      // No need to continue if symbol is hidden by a local definition
      if (strcmp(Symbol::GetName(e->child(Parametric::k_variableIndex)),
                 symbol) != 0) {
        changed = ReplaceSymbol(child, symbol, id + 1, sign) || changed;
      }
    } else {
      changed = ReplaceSymbol(child, symbol, id, sign) || changed;
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
            strcmp(Symbol::GetName(e->child(Parametric::k_variableIndex)),
                   Symbol::GetName(symbol)) == 0)))) {
      changed = ReplaceSymbolWithTree(child, symbol, replacement) || changed;
    }
  }
  if (symbol->isUserNamed() && symbol->nodeIsIdenticalTo(e)) {
    if (symbol->isUserSymbol()) {
      e->cloneTreeOverTree(replacement);
      return true;
    }
    if (symbol->treeIsIdenticalTo(e)) {
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
      ReplaceSymbol(child, e->child(Parametric::k_variableIndex), 0,
                    Parametric::VariableSign(e));
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

bool Variables::HasVariables(const Tree* e) {
  return HasVariablesOutOfScope(e, 0);
}

bool Variables::HasVariable(const Tree* e, const Tree* variable) {
  // TODO: variable must have the same scope as e
  assert(variable->isVar());
  return HasVariable(e, Id(variable));
}

bool Variables::HasVariable(const Tree* e, int id) {
  if (e->isVar()) {
    return Id(e) == id;
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

void Variables::EnterOrLeaveScope(Tree* e, bool enter, int var) {
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

}  // namespace Poincare::Internal
