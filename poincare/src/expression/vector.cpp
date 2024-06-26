#include "vector.h"

#include "k_tree.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

Tree* Vector::Norm(const Tree* e) {
  // Norm is defined on vectors only
  int childrenNumber = e->numberOfChildren();
  Tree* result = KPow->cloneNode();
  Tree* sum = SharedTreeStack->pushAdd(childrenNumber);
  for (const Tree* child : e->children()) {
    Tree* squaredAbsValue = KPow->cloneNode();
    Tree* absValue = KAbs->cloneNode();
    child->cloneTree();
    SystematicReduction::ShallowReduce(absValue);
    (2_e)->cloneTree();
    SystematicReduction::ShallowReduce(squaredAbsValue);
  }
  SystematicReduction::ShallowReduce(sum);
  (1_e / 2_e)->cloneTree();
  SystematicReduction::ShallowReduce(result);
  return result;
}

Tree* Vector::Dot(const Tree* e1, const Tree* e2) {
  // Dot product is defined between two vectors of same size and type
  assert(e1->numberOfChildren() == e2->numberOfChildren());
  int childrenNumber = e2->numberOfChildren();
  Tree* sum = SharedTreeStack->pushAdd(childrenNumber);
  const Tree* childE2 = e2->child(0);
  for (const Tree* childE1 : e1->children()) {
    Tree* product = KMult.node<2>->cloneNode();
    childE1->cloneTree();
    childE2->cloneTree();
    childE2 = childE2->nextTree();
    SystematicReduction::ShallowReduce(product);
  }
  SystematicReduction::ShallowReduce(sum);
  return sum;
}

Tree* Vector::Cross(const Tree* e1, const Tree* e2) {
  // Cross product is defined between two vectors of size 3 and of same type.
  assert(e1->numberOfChildren() == 3 && e2->numberOfChildren() == 3);
  Tree* result = e1->cloneNode();
  for (int j = 0; j < 3; j++) {
    int j1 = (j + 1) % 3;
    int j2 = (j + 2) % 3;
    Tree* difference = KAdd.node<2>->cloneNode();
    Tree* a1b2 = KMult.node<2>->cloneNode();
    e1->child(j1)->cloneTree();
    e2->child(j2)->cloneTree();
    SystematicReduction::ShallowReduce(a1b2);
    Tree* a2b1 = KMult.node<3>->cloneNode();
    (-1_e)->cloneTree();
    e1->child(j2)->cloneTree();
    e2->child(j1)->cloneTree();
    SystematicReduction::ShallowReduce(a2b1);
    SystematicReduction::ShallowReduce(difference);
  }
  return result;
}

}  // namespace Poincare::Internal
