#include "order.h"

#include <poincare/src/memory/node_iterator.h>
#include <poincare/src/memory/type_block.h>

#include "approximation.h"
#include "beautification.h"
#include "dimension.h"
#include "k_tree.h"
#include "polynomial.h"
#include "symbol.h"
#include "variables.h"

namespace Poincare::Internal {

int Order::Compare(const Tree* e0, const Tree* e1, OrderType order) {
  if (order == OrderType::AdditionBeautification) {
    /* Repeat twice, once for symbol degree, once for any degree */
    for (bool sortBySymbolDegree : {true, false}) {
      float n0Degree =
          Beautification::DegreeForSortingAddition(e0, sortBySymbolDegree);
      float n1Degree =
          Beautification::DegreeForSortingAddition(e1, sortBySymbolDegree);
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
    bool e0IsMatrix = Dimension::Get(e0).isMatrix();
    bool e1IsMatrix = Dimension::Get(e1).isMatrix();
    if (e0IsMatrix && e1IsMatrix) {
      if (e0->treeIsIdenticalTo(e1)) {
        return 0;
      }
      return e0 < e1 ? -1 : 1;
    }
    if (e0IsMatrix || e1IsMatrix) {
      // Preserve all matrices to the right
      return e0IsMatrix ? 1 : -1;
    }
    order = OrderType::System;
  }
  TypeBlock type0 = e0->type();
  TypeBlock type1 = e1->type();
  if (type0 > type1) {
    /* We handle this case first to implement only the upper diagonal of the
     * comparison table. */
    return -Compare(e1, e0, order);
  }
  if ((type0.isNumber() && type1.isNumber())) {
    return CompareNumbers(e0, e1);
  }
  if (type0 < type1) {
    /* Note: nodes with a smaller type than Power (numbers and Multiplication)
     * will not benefit from this exception. */
    if (type0 == Type::Pow) {
      if (order == OrderType::Beautification) {
        return -Compare(e0, e1, OrderType::System);
      }
      int comparePowerChild = Compare(e0->child(0), e1, order);
      if (comparePowerChild == 0) {
        // 1/x < x < x^2
        return Compare(e0->child(1), 1_e, order);
      }
      // w^2 < x < y^2
      return comparePowerChild;
    }
    if (type0 == Type::ComplexI) {
      return 1;
    }
    /* Note: nodes with a smaller type than Addition (numbers, Multiplication
     * and Power) / Multiplication (numbers) will not benefit from this
     * exception. */
    if (type0 == Type::Add || type0 == Type::Mult) {
      // sin(x) < (1 + cos(x)) < tan(x) and cos(x) < (sin(x) * tan(x))
      return CompareLastChild(e0, e1);
    }
    return -1;
  }
  assert(type0 == type1);
  if (type0.isUserNamed()) {
    int nameComparison = CompareNames(e0, e1);
    if (nameComparison != 0) {
      // a(x) < b(y)
      return nameComparison;
    }
    // a(1) < a(2), Scan children
  }
  if (type0 == Type::Polynomial) {
    return ComparePolynomial(e0, e1);
  }
  if (type0 == Type::Var) {
    return Variables::Id(e0) - Variables::Id(e1);
  }
  // f(0, 1, 4) < f(0, 2, 3) and (2 + 3) < (1 + 4)
  return CompareChildren(e0, e1, type0 == Type::Add || type0 == Type::Mult);
}

bool Order::ContainsSubtree(const Tree* tree, const Tree* subtree) {
  if (AreEqual(tree, subtree)) {
    return true;
  }
  for (const Tree* child : tree->children()) {
    if (ContainsSubtree(child, subtree)) {
      return true;
    }
  }
  return false;
}

int Order::CompareNumbers(const Tree* e0, const Tree* e1) {
  assert(e0->type() <= e1->type());
  if (e1->isMathematicalConstant()) {
    return e0->isMathematicalConstant() ? CompareConstants(e0, e1) : -1;
  }
  assert(!e0->isMathematicalConstant());
  if (e0->isRational() && e1->isRational()) {
    // TODO_PCJ: return Rational::NaturalOrder(e0, e1);
  }
  float approximation =
      Approximation::To<float>(e0) - Approximation::To<float>(e1);
  if (approximation == 0.0f || std::isnan(approximation)) {
    if (e0->treeIsIdenticalTo(e1)) {
      return 0;
    }
    // Trees are different but float approximation is not precise enough.
    double doubleApproximation =
        Approximation::To<double>(e0) - Approximation::To<double>(e1);
    if (doubleApproximation == 0) {
      return 0;
    }
    return doubleApproximation > 0.0f ? 1 : -1;
  }
  assert(!e0->treeIsIdenticalTo(e1));
  return approximation > 0.0f ? 1 : -1;
}

int Order::CompareNames(const Tree* e0, const Tree* e1) {
  int stringComparison = strcmp(Symbol::GetName(e0), Symbol::GetName(e1));
  if (stringComparison == 0) {
    int delta = Symbol::Length(e0) - Symbol::Length(e1);
    return delta > 0 ? 1 : (delta == 0 ? 0 : -1);
  }
  return stringComparison;
}

int Order::CompareConstants(const Tree* e0, const Tree* e1) {
  return static_cast<uint8_t>(e1->type()) - static_cast<uint8_t>(e0->type());
}

int Order::ComparePolynomial(const Tree* e0, const Tree* e1) {
  int childrenComparison = CompareChildren(e0, e1);
  if (childrenComparison != 0) {
    return childrenComparison;
  }
  int numberOfTerms = Polynomial::NumberOfTerms(e0);
  assert(numberOfTerms == Polynomial::NumberOfTerms(e1));
  for (int i = 0; i < numberOfTerms; i++) {
    uint8_t exponent0 = Polynomial::ExponentAtIndex(e0, i);
    uint8_t exponent1 = Polynomial::ExponentAtIndex(e1, i);
    if (exponent0 != exponent1) {
      return exponent0 - exponent1;
    }
  }
  return 0;
}

int PrivateCompareChildren(const Tree* e0, const Tree* e1) {
  for (std::pair<std::array<const Tree*, 2>, int> indexedNodes :
       MultipleNodesIterator::Children<NoEditable, 2>({e0, e1})) {
    const Tree* child0 = std::get<std::array<const Tree*, 2>>(indexedNodes)[0];
    const Tree* child1 = std::get<std::array<const Tree*, 2>>(indexedNodes)[1];
    int order = Order::Compare(child0, child1);
    if (order != 0) {
      return order;
    }
  }
  return 0;
}

// Use a recursive method to compare the trees backward. Both number of
// nextTree() and comparison is optimal.
int CompareNextTreePairOrItself(const Tree* e0, const Tree* e1,
                                int numberOfComparisons) {
  int nextTreeComparison =
      numberOfComparisons > 1
          ? CompareNextTreePairOrItself(e0->nextTree(), e1->nextTree(),
                                        numberOfComparisons - 1)
          : 0;
  return nextTreeComparison != 0 ? nextTreeComparison : Order::Compare(e0, e1);
}

int PrivateCompareChildrenBackwards(const Tree* e0, const Tree* e1) {
  int numberOfChildren0 = e0->numberOfChildren();
  int numberOfChildren1 = e1->numberOfChildren();
  int numberOfComparisons = std::min(numberOfChildren0, numberOfChildren1);
  if (numberOfComparisons == 0) {
    return 0;
  }
  return CompareNextTreePairOrItself(
      e0->child(numberOfChildren0 - numberOfComparisons),
      e1->child(numberOfChildren1 - numberOfComparisons), numberOfComparisons);
}

int Order::CompareChildren(const Tree* e0, const Tree* e1, bool backward) {
  int comparison = (backward ? PrivateCompareChildrenBackwards
                             : PrivateCompareChildren)(e0, e1);
  if (comparison != 0) {
    return comparison;
  }
  int numberOfChildren0 = e0->numberOfChildren();
  int numberOfChildren1 = e1->numberOfChildren();
  // The NULL node is the least node type.
  if (numberOfChildren0 < numberOfChildren1) {
    return 1;
  }
  if (numberOfChildren1 < numberOfChildren0) {
    return -1;
  }
  return 0;
}

int Order::CompareLastChild(const Tree* e0, const Tree* e1) {
  int comparisonWithChild = Compare(e0->lastChild(), e1);
  if (comparisonWithChild != 0) {
    return comparisonWithChild;
  }
  return 1;
}

bool Order::AreEqual(const Tree* e0, const Tree* e1) {
  // treeIsIdenticalTo is faster since it uses memcmp
  bool areEqual = e0->treeIsIdenticalTo(e1);
  // Trees could be different but compare the same due to float imprecision.
  assert(!areEqual || Compare(e0, e1) == 0);
  return areEqual;
}

}  // namespace Poincare::Internal
