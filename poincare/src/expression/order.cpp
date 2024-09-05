#include "order.h"

#include <poincare/src/memory/multiple_nodes_iterator.h>
#include <poincare/src/memory/type_block.h>

#include "approximation.h"
#include "beautification.h"
#include "dimension.h"
#include "k_tree.h"
#include "polynomial.h"
#include "symbol.h"
#include "unit.h"
#include "variables.h"

namespace Poincare::Internal {

int Order::Compare(const Tree* e1, const Tree* e2, OrderType order) {
  int cmp = memcmp(e1, e2, e1->treeSize());
  if (cmp == 0) {
    return 0;
  }
  int result = CompareDifferent(e1, e2, order);
  // Fallback to cmp if no difference could be found.
  result = result == 0 ? cmp : result;
  // Sanitize result to -1, 0 or 1.
  return result < 0 ? -1 : (result > 0 ? 1 : 0);
}

int Order::CompareDifferent(const Tree* e1, const Tree* e2, OrderType order) {
  if (order == OrderType::AdditionBeautification) {
    /* Repeat twice, once for symbol degree, once for any degree */
    for (bool sortBySymbolDegree : {true, false}) {
      float n0Degree =
          Beautification::DegreeForSortingAddition(e1, sortBySymbolDegree);
      float n1Degree =
          Beautification::DegreeForSortingAddition(e2, sortBySymbolDegree);
      if (!std::isnan(n1Degree) &&
          (std::isnan(n0Degree) || n0Degree > n1Degree)) {
        return -1;
      }
      if (!std::isnan(n0Degree) &&
          (std::isnan(n1Degree) || n1Degree > n0Degree)) {
        return 1;
      }
    }
    // If they have same degree, sort children in decreasing order of base.
    order = OrderType::Beautification;
  }
  if (order == OrderType::PreserveMatrices) {
    bool e1IsMatrix = Dimension::Get(e1).isMatrix();
    bool e2IsMatrix = Dimension::Get(e2).isMatrix();
    if (e1IsMatrix && e2IsMatrix) {
      return e1 - e2;
    }
    if (e1IsMatrix || e2IsMatrix) {
      // Preserve all matrices to the right
      return e1IsMatrix ? 1 : -1;
    }
    order = OrderType::System;
  }
  TypeBlock type1 = e1->type();
  TypeBlock type2 = e2->type();
  if (type1 > type2) {
    /* We handle this case first to implement only the upper diagonal of the
     * comparison table. */
    return -CompareDifferent(e2, e1, order);
  }
  if ((type1.isNumber() && type2.isNumber())) {
    return CompareNumbers(e1, e2);
  }
  if (type1 < type2) {
    /* Note: nodes with a smaller type than Power (numbers and Multiplication)
     * will not benefit from this exception. */
    if (type1 == Type::Pow) {
      if (order == OrderType::Beautification) {
        return -CompareDifferent(e1, e2, OrderType::System);
      }
      int comparePowerChild = Compare(e1->child(0), e2, order);
      if (comparePowerChild == 0) {
        // 1/x < x < x^2
        return Compare(e1->child(1), 1_e, order);
      }
      // w^2 < x < y^2
      return comparePowerChild;
    }
    if (type1 == Type::ComplexI) {
      return 1;
    }
    /* Note: nodes with a smaller type than Addition (numbers, Multiplication
     * and Power) / Multiplication (numbers) will not benefit from this
     * exception. */
    if (type1 == Type::Add || type1 == Type::Mult) {
      // sin(x) < (1 + cos(x)) < tan(x) and cos(x) < (sin(x) * tan(x))
      return CompareLastChild(e1, e2);
    }
    return -1;
  }
  assert(type1 == type2);
  if (type1.isUserNamed()) {
    int nameComparison = CompareNames(e1, e2);
    if (nameComparison != 0) {
      // a(x) < b(y)
      return nameComparison;
    }
    // a(1) < a(2), Scan children
  }
  if (type1 == Type::Polynomial) {
    return ComparePolynomial(e1, e2);
  }
  if (type1 == Type::Var) {
    return Variables::Id(e1) - Variables::Id(e2);
  }
  if (type1 == Type::Unit) {
    // TODO: Only with Beautification OrderType or maybe AdditionBeautification.
    // Sort units from biggest to smallest ratio for implicit additions.
    const Units::Representative* repr1 = Units::Unit::GetRepresentative(e1);
    const Units::Representative* repr2 = Units::Unit::GetRepresentative(e2);
    if (repr1 == repr2) {
      // Prefix could be used, but there is no needs for a specific order.
      return 0;
    }
    // Sort units from biggest to smallest ratio.
    float ratioDiff = repr2->ratio() - repr1->ratio();
    return ratioDiff < 0.f ? -1 : ratioDiff == 0.f ? 0 : 1;
  }
  /* f(0, 1, 4) < f(0, 2, 3)
   * (2 + 3) < (1 + 4)
   * trig(5, 0) < trig(4, 1)   (same order as Type::Cos and Type::Sin)
   * same with atrig */
  return CompareChildren(e1, e2,
                         type1 == Type::Add || type1 == Type::Mult ||
                             type1 == Type::Trig || type1 == Type::ATrig);
}

bool Order::ContainsSubtree(const Tree* tree, const Tree* subtree) {
  if (tree->treeIsIdenticalTo(subtree)) {
    return true;
  }
  for (const Tree* child : tree->children()) {
    if (ContainsSubtree(child, subtree)) {
      return true;
    }
  }
  return false;
}

int Order::CompareNumbers(const Tree* e1, const Tree* e2) {
  assert(e1->type() <= e2->type());
  if (e2->isMathematicalConstant()) {
    return e1->isMathematicalConstant() ? CompareConstants(e1, e2) : -1;
  }
  assert(!e1->isMathematicalConstant());
  if (e1->isRational() && e2->isRational()) {
    // TODO_PCJ: return Rational::NaturalOrder(e1, e2);
  }
  float approximation =
      Approximation::To<float>(e1) - Approximation::To<float>(e2);
  if (approximation == 0.0f || std::isnan(approximation)) {
    // Trees are different but float approximation is not precise enough.
    double doubleApprox =
        Approximation::To<double>(e1) - Approximation::To<double>(e2);
    return doubleApprox < 0.0 ? -1 : (doubleApprox > 0.0 ? 1 : 0);
  }
  return approximation < 0.0 ? -1 : (approximation > 0.0 ? 1 : 0);
}

int Order::CompareNames(const Tree* e1, const Tree* e2) {
  int stringComparison = strcmp(Symbol::GetName(e1), Symbol::GetName(e2));
  if (stringComparison == 0) {
    return Symbol::Length(e1) - Symbol::Length(e2);
  }
  return stringComparison;
}

int Order::CompareConstants(const Tree* e1, const Tree* e2) {
  return static_cast<uint8_t>(e2->type()) - static_cast<uint8_t>(e1->type());
}

int Order::ComparePolynomial(const Tree* e1, const Tree* e2) {
  int childrenComparison = CompareChildren(e1, e2);
  if (childrenComparison != 0) {
    return childrenComparison;
  }
  int numberOfTerms = Polynomial::NumberOfTerms(e1);
  assert(numberOfTerms == Polynomial::NumberOfTerms(e2));
  for (int i = 0; i < numberOfTerms; i++) {
    uint8_t exponent1 = Polynomial::ExponentAtIndex(e1, i);
    uint8_t exponent2 = Polynomial::ExponentAtIndex(e2, i);
    if (exponent1 != exponent2) {
      return exponent1 - exponent2;
    }
  }
  return 0;
}

int PrivateCompareChildren(const Tree* e1, const Tree* e2) {
  for (std::pair<std::array<const Tree*, 2>, int> indexedNodes :
       MultipleNodesIterator::Children<NoEditable, 2>({e1, e2})) {
    const Tree* child1 = std::get<std::array<const Tree*, 2>>(indexedNodes)[0];
    const Tree* child2 = std::get<std::array<const Tree*, 2>>(indexedNodes)[1];
    int order = Order::Compare(child1, child2);
    if (order != 0) {
      return order;
    }
  }
  return 0;
}

// Use a recursive method to compare the trees backward. Both number of
// nextTree() and comparison is optimal.
int CompareNextTreePairOrItself(const Tree* e1, const Tree* e2,
                                int numberOfComparisons) {
  int nextTreeComparison =
      numberOfComparisons > 1
          ? CompareNextTreePairOrItself(e1->nextTree(), e2->nextTree(),
                                        numberOfComparisons - 1)
          : 0;
  return nextTreeComparison != 0 ? nextTreeComparison : Order::Compare(e1, e2);
}

int PrivateCompareChildrenBackwards(const Tree* e1, const Tree* e2) {
  int numberOfChildren1 = e1->numberOfChildren();
  int numberOfChildren2 = e2->numberOfChildren();
  int numberOfComparisons = std::min(numberOfChildren1, numberOfChildren2);
  if (numberOfComparisons == 0) {
    return 0;
  }
  return CompareNextTreePairOrItself(
      e1->child(numberOfChildren1 - numberOfComparisons),
      e2->child(numberOfChildren2 - numberOfComparisons), numberOfComparisons);
}

int Order::CompareChildren(const Tree* e1, const Tree* e2, bool backward) {
  int comparison = (backward ? PrivateCompareChildrenBackwards
                             : PrivateCompareChildren)(e1, e2);
  if (comparison != 0) {
    return comparison;
  }
  // The NULL node is the least node type.
  return e2->numberOfChildren() - e1->numberOfChildren();
}

int Order::CompareLastChild(const Tree* e1, const Tree* e2) {
  return Compare(e1->lastChild(), e2) == -1 ? -1 : 1;
}

}  // namespace Poincare::Internal
