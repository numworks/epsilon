#include "variables.h"

#include <poincare_junior/src/memory/edition_pool.h>

#include "k_tree.h"
#include "set.h"
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

Tree* Variables::GetVariables(const Tree* expr) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = Set::PushEmpty();
  for (const Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::UserSymbol) {
      Set::Add(set, child);
    }
  }
  return set;
}

void Variables::ProjectToId(Tree* expr, const Tree* variables) {
  assert(SharedEditionPool->isAfter(variables, expr));
  for (Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::UserSymbol) {
      Tree* var = SharedEditionPool->push<BlockType::Variable>(
          ToId(variables, Symbol::NonNullTerminatedName(child),
               Symbol::Length(child)));
      child->moveTreeOverTree(var);
    }
  }
}

void Variables::BeautifyToName(Tree* expr, const Tree* variables) {
  assert(SharedEditionPool->isAfter(variables, expr));
  for (Tree* child : expr->selfAndDescendants()) {
    if (child->type() == BlockType::Variable) {
      child->cloneTreeOverTree(Variables::ToSymbol(variables, Id(child)));
    }
  }
}

}  // namespace PoincareJ
