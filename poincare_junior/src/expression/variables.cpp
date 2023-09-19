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
  return variables->childAtIndex(id);
}

Tree* Variables::GetUserSymbols(const Tree* expr) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  GetUserSymbols(expr, set);
  return set;
}

void Variables::GetUserSymbols(const Tree* expr, Tree* set) {
  if (expr->type() == BlockType::UserSymbol) {
    Set::Add(set, expr);
  } else if (expr->type().isParametric()) {
    Tree* subSet = Set::PushEmpty();
    GetUserSymbols(expr->childAtIndex(Parametric::k_functionIndex), subSet);
    Tree* boundSymbols = Set::PushEmpty();
    Set::Add(boundSymbols, expr->childAtIndex(Parametric::k_variableIndex));
    subSet = Set::Difference(subSet, boundSymbols);
    set = Set::Union(set, subSet);
  } else {
    for (const Tree* child : expr->children()) {
      GetUserSymbols(child, set);
    }
  }
}

bool Variables::Replace(Tree* expr, const Tree* variable, const Tree* value) {
  assert(variable->type() == BlockType::Variable);
  return Replace(expr, Id(variable), value);
}

bool Variables::Replace(Tree* expr, int id, const Tree* value) {
  if (expr->type() == BlockType::Variable && Id(expr) == id) {
    expr->cloneTreeOverTree(value);
    return true;
  }
  if (expr->type().isParametric()) {
    id++;
  }
  bool changed = false;
  for (Tree* child : expr->children()) {
    changed = Replace(child, id, value) || changed;
  }
  if (changed) {
    Simplification::ShallowSystematicReduce(expr);
  }
  return changed;
}

bool Variables::ReplaceSymbol(Tree* expr, const Tree* symbol, int id) {
  if (expr->type() == BlockType::UserSymbol &&
      Symbol::Length(expr) == Symbol::Length(symbol) &&
      strncmp(Symbol::NonNullTerminatedName(expr),
              Symbol::NonNullTerminatedName(symbol),
              Symbol::Length(symbol)) == 0) {
    Tree* var =
        SharedEditionPool->push<BlockType::Variable>(static_cast<uint8_t>(id));
    expr->moveTreeOverTree(var);
    return true;
  }
  if (expr->type().isParametric()) {
    Tree* child = expr->childAtIndex(Parametric::k_functionIndex);
    if (Symbol::Length(child) == Symbol::Length(symbol) &&
        strncmp(Symbol::NonNullTerminatedName(child),
                Symbol::NonNullTerminatedName(symbol),
                Symbol::Length(symbol)) == 0) {
      return false;
    }
    id++;
  }
  bool changed = false;
  for (Tree* child : expr->children()) {
    changed = ReplaceSymbol(child, symbol, id) || changed;
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
  } else if (expr->type().isParametric()) {
    Tree* child = expr->childAtIndex(Parametric::k_functionIndex);
    ReplaceSymbol(child, expr->childAtIndex(Parametric::k_variableIndex), 0);
    ProjectToId(child, variables, depth + 1);
    // TODO bounds
    return;
  }
  for (Tree* child : expr->children()) {
    ProjectToId(child, variables, depth);
  }
}

void Variables::BeautifyToName(Tree* expr, const Tree* variables,
                               uint8_t depth) {
  assert(SharedEditionPool->isAfter(variables, expr));
  if (expr->type() == BlockType::Variable && depth <= Id(expr)) {
    expr->cloneTreeOverTree(Variables::ToSymbol(variables, Id(expr) - depth));
  } else if (expr->type().isParametric()) {
    Tree* child = expr->childAtIndex(Parametric::k_functionIndex);
    // beautify outer variables
    BeautifyToName(child, variables, depth + 1);
    // beautify variable introduced by this scope
    // TODO check that name is available here or make new name
    Variables::Replace(child, 0,
                       expr->childAtIndex(Parametric::k_variableIndex));
    return;
  }
  for (Tree* child : expr->children()) {
    BeautifyToName(child, variables, depth);
  }
}

bool Variables::HasVariables(const Tree* expr) {
  for (const Tree* child : expr->selfAndDescendants()) {
    assert(child->type() != BlockType::UserSymbol);
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
  assert(expr->type() != BlockType::UserSymbol);
  if (expr->type() == BlockType::Variable) {
    return Id(expr) == id;
  } else if (expr->type().isParametric()) {
    id++;
  }
  for (const Tree* child : expr->children()) {
    if (HasVariable(child, id)) {
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
