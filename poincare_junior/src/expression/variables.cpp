#include "variables.h"

#include <poincare_junior/src/memory/edition_pool.h>

#include "k_tree.h"
#include "parametric.h"
#include "set.h"
#include "simplification.h"
#include "symbol.h"

namespace PoincareJ {

Variables::Variable::Variable(uint8_t id) {
  Tree* temp = SharedEditionPool->push<BlockType::Variable>(id);
  assert(k_size == temp->treeSize());
  temp->copyTreeTo(m_blocks);
  temp->removeTree();
}

uint8_t Variables::Id(const Tree* variable) {
  assert(variable->type() == BlockType::Variable);
  return variable->nodeValue(0);
}

uint8_t Variables::ToId(const Tree* variables, const char* name,
                        uint8_t length) {
  for (uint8_t i = 0; const Tree* child : variables->children()) {
    if (Symbol::Length(child) == length &&
        strncmp(Symbol::NonNullTerminatedName(child), name, length) == 0) {
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
  if (expr->type() == BlockType::UserSymbol) {
    return Set::Add(set, expr);
  }
  bool isParametric = expr->type().isParametric();
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

bool Variables::Replace(Tree* expr, const Tree* variable, const Tree* value) {
  assert(variable->type() == BlockType::Variable);
  return Replace(expr, Id(variable), value);
}

bool Variables::Replace(Tree* expr, int id, const Tree* value, bool leave) {
  if (expr->type() == BlockType::Variable) {
    if (Id(expr) == id) {
      expr->cloneTreeOverTree(value);
      return true;
    }
    if (leave) {
      expr->setNodeValue(0, Id(expr) - 1);
      return true;
    }
    return false;
  }
  bool isParametric = expr->type().isParametric();
  bool changed = false;
  for (int i = 0; Tree * child : expr->children()) {
    int updatedId =
        id + (isParametric && i++ == Parametric::FunctionIndex(expr));
    changed = Replace(child, updatedId, value, leave) || changed;
  }
  if (changed) {
    Simplification::ShallowSystematicReduce(expr);
  }
  return changed;
}

bool Variables::ReplaceSymbol(Tree* expr, const Tree* symbol, int id) {
  if (expr->type() == BlockType::UserSymbol &&
      expr->treeIsIdenticalTo(symbol)) {
    Tree* var =
        SharedEditionPool->push<BlockType::Variable>(static_cast<uint8_t>(id));
    expr->moveTreeOverTree(var);
    return true;
  }
  bool isParametric = expr->type().isParametric();
  bool changed = false;
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i == Parametric::k_variableIndex) {
    } else if (isParametric && i == Parametric::FunctionIndex(expr)) {
      Tree* newSymbol = expr->child(Parametric::k_variableIndex);
      // No need to continue if symbol is hidden by a local definition
      if (!newSymbol->treeIsIdenticalTo(symbol)) {
        changed = ReplaceSymbol(child, symbol, id + 1) || changed;
      }
    } else {
      changed = ReplaceSymbol(child, symbol, id) || changed;
    }
    i++;
  }
  return changed;
}

void Variables::ProjectToId(Tree* expr, const Tree* variables, uint8_t depth) {
  assert(SharedEditionPool->isAfter(variables, expr));
  if (expr->type() == BlockType::UserSymbol) {
    Tree* var =
        SharedEditionPool->push<BlockType::Variable>(static_cast<uint8_t>(
            ToId(variables, Symbol::NonNullTerminatedName(expr),
                 Symbol::Length(expr)) +
            depth));
    expr->moveTreeOverTree(var);
  }
  bool isParametric = expr->type().isParametric();
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i == Parametric::k_variableIndex) {
    } else if (isParametric && i == Parametric::FunctionIndex(expr)) {
      ReplaceSymbol(child, expr->child(Parametric::k_variableIndex), 0);
      ProjectToId(child, variables, depth + 1);
    } else {
      ProjectToId(child, variables, depth);
    }
    i++;
  }
}

void Variables::BeautifyToName(Tree* expr, const Tree* variables,
                               uint8_t depth) {
  assert(SharedEditionPool->isAfter(variables, expr));
  if (expr->type() == BlockType::Variable) {
    assert(depth <= Id(expr));
    expr->cloneTreeOverTree(Variables::ToSymbol(variables, Id(expr) - depth));
  }
  bool isParametric = expr->type().isParametric();
  for (int i = 0; Tree * child : expr->children()) {
    if (isParametric && i++ == Parametric::FunctionIndex(expr)) {
      // beautify variable introduced by this scope
      // TODO check that name is available here or make new name
      Variables::Replace(child, 0, expr->child(Parametric::k_variableIndex));
      // beautify outer variables
      BeautifyToName(child, variables, depth + 1);
      continue;
    }
    BeautifyToName(child, variables, depth);
  }
}

bool Variables::HasVariables(const Tree* expr) {
  for (const Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::Variable) {
      return true;
    }
  }
  return false;
}

bool Variables::HasVariable(const Tree* expr, const Tree* variable) {
  // TODO variable must have the same scope as expr
  assert(variable->type() == BlockType::Variable);
  return HasVariable(expr, Id(variable));
}

bool Variables::HasVariable(const Tree* expr, int id) {
  if (expr->type() == BlockType::Variable) {
    return Id(expr) == id;
  }
  bool isParametric = expr->type().isParametric();
  for (int i = 0; const Tree* child : expr->children()) {
    int updatedId =
        id + (isParametric && i++ == Parametric::FunctionIndex(expr));
    if (HasVariable(child, updatedId)) {
      return true;
    }
  }
  return false;
}

void Variables::EnterScope(Tree* expr) {
  for (Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::Variable) {
      uint8_t id = Id(child);
      assert(id < 255);
      child->setNodeValue(0, id + 1);
    }
  }
}

void Variables::LeaveScope(Tree* expr) {
  for (Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::Variable) {
      uint8_t id = Id(child);
      assert(id > 0);
      child->setNodeValue(0, id - 1);
    }
  }
}

}  // namespace PoincareJ
