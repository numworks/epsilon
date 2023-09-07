#include "variables.h"

#include "k_tree.h"
#include "set.h"

namespace PoincareJ {

Tree* Variables::GetVariables(const Tree* t) {
  // TODO Is it worth to represent the empty set with nullptr ?
  Tree* set = KSet.node<0>->cloneNode();
  for (const Tree* child : t->descendants()) {
    if (child->type() == BlockType::UserSymbol) {
      Set::Add(set, child);
    }
  }
  return set;
}

}  // namespace PoincareJ
