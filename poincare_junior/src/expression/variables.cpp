#include "variables.h"

#include "k_tree.h"
#include "set.h"
#include "symbol.h"

namespace PoincareJ {

uint8_t Variables::Id(const Tree* variable) {
  assert(variable->type() == BlockType::Variable);
  return static_cast<uint8_t>(variable->block(1));
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

Tree* Variables::GetVariables(const Tree* t) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  for (const Tree* child : t->descendants()) {
    if (child->type() == BlockType::UserSymbol) {
      Set::Add(set, child);
    }
  }
  return set;
}

void Variables::ProjectToId(Tree* expr, const Tree* variables) {
  assert(SharedEditionPool->isAfter(variables, expr));
  Tree* child = expr;
  int remainingNodes = 1;
  while (remainingNodes) {
    if (child->type() == BlockType::UserSymbol) {
      Tree* var = SharedEditionPool->push<BlockType::Variable>(
          ToId(variables, Symbol::NonNullTerminatedName(child),
               Symbol::Length(child)));
      child->moveNodeOverNode(var);
    }
    remainingNodes += child->numberOfChildren() - 1;
    child = child->nextNode();
  }
}

void Variables::BeautifyToName(Tree* expr, const Tree* variables) {
  assert(SharedEditionPool->isAfter(variables, expr));
  Tree* child = expr;
  int remainingNodes = 1;
  while (remainingNodes) {
    if (child->type() == BlockType::Variable) {
      child->cloneNodeOverNode(Variables::ToSymbol(variables, Id(child)));
    }
    remainingNodes += child->numberOfChildren() - 1;
    child = child->nextNode();
  }
}

}  // namespace PoincareJ
