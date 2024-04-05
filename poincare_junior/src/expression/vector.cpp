#include "vector.h"

#include "k_tree.h"
#include "simplification.h"

namespace PoincareJ {

Tree* Vector::Norm(const Tree* v) {
  // Norm is defined on vectors only
  int childrenNumber = v->numberOfChildren();
  Tree* result = KPow->cloneNode();
  Tree* sum = SharedTreeStack->push<Type::Addition>(childrenNumber);
  for (const Tree* child : v->children()) {
    Tree* squaredAbsValue = KPow->cloneNode();
    Tree* absValue = KAbs->cloneNode();
    child->clone();
    Simplification::ShallowSystematicReduce(absValue);
    (2_e)->clone();
    Simplification::ShallowSystematicReduce(squaredAbsValue);
  }
  Simplification::ShallowSystematicReduce(sum);
  KHalf->clone();
  Simplification::ShallowSystematicReduce(result);
  return result;
}

Tree* Vector::Dot(const Tree* u, const Tree* v) {
  // Dot product is defined between two vectors of same size and type
  assert(u->numberOfChildren() == v->numberOfChildren());
  int childrenNumber = v->numberOfChildren();
  Tree* sum = SharedTreeStack->push<Type::Addition>(childrenNumber);
  const Tree* childV = v->nextNode();
  for (const Tree* childU : u->children()) {
    Tree* product = KMult.node<2>->cloneNode();
    childU->clone();
    childV->clone();
    childV = childV->nextTree();
    Simplification::ShallowSystematicReduce(product);
  }
  Simplification::ShallowSystematicReduce(sum);
  return sum;
}

Tree* Vector::Cross(const Tree* u, const Tree* v) {
  // Cross product is defined between two vectors of size 3 and of same type.
  assert(u->numberOfChildren() == 3 && v->numberOfChildren() == 3);
  Tree* result = u->cloneNode();
  for (int j = 0; j < 3; j++) {
    int j1 = (j + 1) % 3;
    int j2 = (j + 2) % 3;
    Tree* difference = KAdd.node<2>->cloneNode();
    Tree* a1b2 = KMult.node<2>->cloneNode();
    u->child(j1)->clone();
    v->child(j2)->clone();
    Simplification::ShallowSystematicReduce(a1b2);
    Tree* a2b1 = KMult.node<3>->cloneNode();
    (-1_e)->clone();
    u->child(j2)->clone();
    v->child(j1)->clone();
    Simplification::ShallowSystematicReduce(a2b1);
    Simplification::ShallowSystematicReduce(difference);
  }
  return result;
}

}  // namespace PoincareJ
