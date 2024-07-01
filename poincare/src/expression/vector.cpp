#include "vector.h"

#include "k_tree.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

static bool IsVector(const Tree* e) {
  return e->isMatrix() &&
         (Matrix::NumberOfRows(e) == 1 || Matrix::NumberOfColumns(e) == 1);
}

Tree* Vector::Norm(const Tree* e) {
  // Norm is defined on vectors only
  assert(IsVector(e));
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
  assert(IsVector(e1) && IsVector(e2));
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
  assert(IsVector(e1) && IsVector(e2));
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

#if 0
// TODO_PCJ: approximations

template <typename T>
std::complex<T> MatrixComplexNode<T>::norm() const {
  if (!isVector()) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> sum = 0;
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    sum += std::norm(complexAtIndex(i));
  }
  return std::sqrt(sum);
}

template <typename T>
std::complex<T> MatrixComplexNode<T>::dot(MatrixComplex<T> *e) const {
  if (!isVector() || vectorType() != e->vectorType() ||
      numberOfChildren() != e->numberOfChildren()) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> sum = 0;
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    sum += complexAtIndex(i) * e->complexAtIndex(i);
  }
  return sum;
}

template <typename T>
Evaluation<T> MatrixComplexNode<T>::cross(MatrixComplex<T> *e) const {
  if (!isVector() || vectorType() != e->vectorType() ||
      numberOfChildren() != 3 || e->numberOfChildren() != 3) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[3];
  operandsCopy[0] = complexAtIndex(1) * e->complexAtIndex(2) -
                    complexAtIndex(2) * e->complexAtIndex(1);
  operandsCopy[1] = complexAtIndex(2) * e->complexAtIndex(0) -
                    complexAtIndex(0) * e->complexAtIndex(2);
  operandsCopy[2] = complexAtIndex(0) * e->complexAtIndex(1) -
                    complexAtIndex(1) * e->complexAtIndex(0);
  return MatrixComplex<T>::Builder(operandsCopy, numberOfRows(),
                                   numberOfColumns());
}
#endif

}  // namespace Poincare::Internal
