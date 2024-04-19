#include "variables.h"

#include <poincare/src/memory/tree_stack.h>
#include <string.h>

#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "simplification.h"
#include "symbol.h"

namespace Poincare::Internal {

Variables::Variable::Variable(uint8_t id, ComplexSign sign) {
  Tree* temp = SharedTreeStack->push<Type::Var>(id, sign);
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
  return ComplexSign(variable->nodeValue(1));
}

uint8_t Variables::ToId(const Tree* variables, const char* name,
                        uint8_t length) {
  for (uint8_t i = 0; const Tree* child : variables->children()) {
    if (strcmp(Symbol::GetName(child), name) == 0) {
      return i;
    }
    i++;
  }
  assert(false);  // Not found
}

const Tree* Variables::ToSymbol(const Tree* variables, uint8_t id) {
  return variables->child(id);
}

Tree* Variables::GetUserSymbols(const Tree* expr) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  GetUserSymbols(expr, set);
  return set;
}

void Variables::GetUserSymbols(const Tree* expr, Tree* set) {
  if (expr->isUserSymbol()) {
    return Set::Add(set, expr);
  }
  bool isParametric = expr->isParametric();
  for (int i = 0; const Tree* child : expr->children()) {
    if (isParametric && i == Parametric::k_variableIndex) {
    } else if (isParametric && i == Parametric::FunctionIndex(expr)) {
      Tree* subSet = Set::PushEmpty();
      GetUserSymbols(child, subSet);
      Tree* boundSymbols = Set::PushEmpty();
      Set::Add(boundSymbols, expr->child(Parametric::k_variableIndex));
      subSet = Set::Difference(subSet, boundSymbols);
      set = Set::Union(set, subSet);
    } else {
      GetUserSymbols(child, set);
    }
    i++;
  }
}

bool Variables::Replace(Tree* expr, const Tree* variable, const Tree* value,
                        bool simplify) {
  assert(variable->isVar());
  return Replace(expr, Id(variable), value, simplify);
}

bool Variables::Replace(Tree* expr, int id, const Tree* value, bool leave,
                        bool simplify) {
  /* TODO We need to track the replacement value only if it is in the pool and
   * after expr. Cloning is overkill but easy. */
  TreeRef valueRef = value->clone();
  bool result = Replace(expr, id, valueRef, leave, simplify);
  valueRef->removeTree();
  return result;
}

bool Variables::Replace(Tree* expr, int id, const TreeRef& value, bool leave,
                        bool simplify) {
  if (expr->isVar()) {
    if (Id(expr) == id) {
      expr->cloneTreeOverTree(value);
      return true;
    }
    if (leave && Id(expr) > id) {
      expr->setNodeValue(0, Id(expr) - 1);
      return true;
    }
    return false;
  }
  bool isParametric = expr->isParametric();
  bool changed = false;
  for (int i = 0; Tree * child : expr->children()) {
    int updatedId =
        id + (isParametric && i++ == Parametric::FunctionIndex(expr));
    changed = Replace(child, updatedId, value, leave, simplify) || changed;
  }
  if (simplify && changed) {
    Simplification::ShallowSystematicReduce(expr);
  }
  return changed;
}

bool Variables::ReplaceSymbol(Tree* expr, const Tree* symbol, int id,
                              ComplexSign sign) {
  assert(symbol->isUserSymbol());
  return ReplaceSymbol(expr, Symbol::GetName(symbol), id, sign);
}

bool Variables::ReplaceSymbol(Tree* expr, const char* symbol, int id,
                              ComplexSign sign) {
  if (expr->isUserSymbol() && strcmp(Symbol::GetName(expr), symbol) == 0) {
    Tree* var =
        SharedTreeStack->push<Type::Var>(static_cast<uint8_t>(id), sign);
    expr->moveTreeOverTree(var);
    return true;
  }
  bool isParametric = expr->isParametric();
  bool changed = false;
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i == Parametric::k_variableIndex) {
    } else if (isParametric && i == Parametric::FunctionIndex(expr)) {
      Tree* newSymbol = expr->child(Parametric::k_variableIndex);
      // No need to continue if symbol is hidden by a local definition
      if (strcmp(Symbol::GetName(newSymbol), symbol) != 0) {
        changed = ReplaceSymbol(child, symbol, id + 1, sign) || changed;
      }
    } else {
      changed = ReplaceSymbol(child, symbol, id, sign) || changed;
    }
    i++;
  }
  return changed;
}

bool Variables::ProjectLocalVariablesToId(Tree* expr, uint8_t depth) {
  bool changed = false;
  bool isParametric = expr->isParametric();
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i == Parametric::k_variableIndex) {
    } else if (isParametric && i == Parametric::FunctionIndex(expr)) {
      // Project local variable
      ReplaceSymbol(child, expr->child(Parametric::k_variableIndex), 0,
                    Parametric::VariableSign(expr));
      changed = ProjectLocalVariablesToId(child, depth + 1) || changed;
    } else {
      changed = ProjectLocalVariablesToId(child, depth) || changed;
    }
    i++;
  }
  return changed;
}

bool Variables::BeautifyToName(Tree* expr, uint8_t depth) {
  assert(!expr->isVar());
  bool changed = false;
  bool isParametric = expr->isParametric();
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i++ == Parametric::FunctionIndex(expr)) {
      // beautify variable introduced by this scope
      // TODO check that name is available here or make new name
      changed = Replace(child, 0, expr->child(Parametric::k_variableIndex)) ||
                changed;
      // beautify outer variables
      changed = BeautifyToName(child, depth + 1) || changed;
      continue;
    }
    changed = BeautifyToName(child, depth) || changed;
  }
  return changed;
}

bool Variables::HasVariables(const Tree* expr) {
  // TODO we probably want to ignore bound variables
  // return HasVariable(expr, 0) ?
  return expr->hasDescendantSatisfying(
      [](const Tree* e) { return e->isVar(); });
}

bool Variables::HasVariable(const Tree* expr, const Tree* variable) {
  // TODO variable must have the same scope as expr
  assert(variable->isVar());
  return HasVariable(expr, Id(variable));
}

bool Variables::HasVariable(const Tree* expr, int id) {
  if (expr->isVar()) {
    return Id(expr) == id;
  }
  bool isParametric = expr->isParametric();
  for (int i = 0; const Tree* child : expr->children()) {
    int updatedId =
        id + (isParametric && i++ == Parametric::FunctionIndex(expr));
    if (HasVariable(child, updatedId)) {
      return true;
    }
  }
  return false;
}

void Variables::EnterOrLeaveScope(Tree* expr, bool enter, int var) {
  if (expr->isVar()) {
    uint8_t id = Id(expr);
    if (id > var) {
      assert(enter || id > 0);
      expr->setNodeValue(0, enter ? id + 1 : id - 1);
    }
    return;
  }
  bool isParametric = expr->isParametric();
  for (int i = 0; Tree * child : expr->children()) {
    int updatedId =
        var + (isParametric && i++ == Parametric::FunctionIndex(expr));
    EnterOrLeaveScope(child, enter, updatedId);
  }
}

}  // namespace Poincare::Internal
