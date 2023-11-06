#include "comparison.h"

#include <poincare_junior/src/memory/node_iterator.h>

#include "approximation.h"
#include "beautification.h"
#include "constant.h"
#include "dimension.h"
#include "k_tree.h"
#include "poincare_junior/src/memory/type_block.h"
#include "polynomial.h"
#include "symbol.h"
#include "variables.h"

namespace PoincareJ {

int Comparison::Compare(const Tree* node0, const Tree* node1, Order order) {
  if (order == Order::AdditionBeautification) {
    /* Repeat twice, once for symbol degree, once for any degree */
    for (bool sortBySymbolDegree : {true, false}) {
      float n0Degree =
          Beautification::DegreeForSortingAddition(node0, sortBySymbolDegree);
      float n1Degree =
          Beautification::DegreeForSortingAddition(node1, sortBySymbolDegree);
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
    order = Order::Beautification;
  }
  if (order == Order::PreserveMatrices) {
    if (Dimension::GetDimension(node0).isMatrix() &&
        Dimension::GetDimension(node1).isMatrix()) {
      if (node0->treeIsIdenticalTo(node1)) {
        return 0;
      }
      return node0 < node1 ? -1 : 1;
    }
    order = Order::System;
  }
  TypeBlock type0 = node0->type();
  TypeBlock type1 = node1->type();
  if (type0 > type1) {
    /* We handle this case first to implement only the upper diagonal of the
     * comparison table. */
    return -Compare(node1, node0);
  }
  if ((type0.isNumber() && type1.isNumber())) {
    return CompareNumbers(node0, node1);
  }
  if (type0 < type1) {
    /* Note: nodes with a smaller type than Power (numbers and Multiplication)
     * will not benefit from this exception. */
    if (type0 == BlockType::Power) {
      if (order == Order::Beautification) {
        return -Compare(node0, node1, Order::System);
      }
      if (Compare(node0->child(0), node1) == 0) {
        // 1/x < x < x^2
        return Compare(node0->child(1), 1_e);
      }
      // x < y^2
      return 1;
    }
    if (type0 == BlockType::Constant &&
        Constant::Type(node0) == Constant::Type::I) {
      return 1;
    }
    /* Note: nodes with a smaller type than Addition (numbers, Multiplication
     * and Power) / Multiplication (numbers) will not benefit from this
     * exception. */
    if (type0 == BlockType::Addition || type0 == BlockType::Multiplication) {
      // sin(x) < (1 + cos(x)) < tan(x) and cos(x) < (sin(x) * tan(x))
      return CompareLastChild(node0, node1);
    }
    return -1;
  }
  assert(type0 == type1);
  if (type0.isUserNamed()) {
    int nameComparison = CompareNames(node0, node1);
    if (nameComparison != 0) {
      // a(x) < b(y)
      return nameComparison;
    }
    // a(1) < a(2), Scan children
  }
  if (type0 == BlockType::Polynomial) {
    return ComparePolynomial(node0, node1);
  }
  if (type0 == BlockType::Variable) {
    return Variables::Id(node0) - Variables::Id(node1);
  }
  // f(0, 1, 4) < f(0, 2, 3) and (2 + 3) < (1 + 4)
  return CompareChildren(
      node0, node1,
      type0 == BlockType::Addition || type0 == BlockType::Multiplication);
}

bool Comparison::ContainsSubtree(const Tree* tree, const Tree* subtree) {
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

int Comparison::CompareNumbers(const Tree* node0, const Tree* node1) {
  assert(node0->type() <= node1->type());
  if (node1->isConstant()) {
    return node0->isConstant() ? CompareConstants(node0, node1) : -1;
  }
  assert(!node0->isConstant());
  if (node0->isRational() && node1->isRational()) {
    // TODO
    // return Rational::NaturalOrder(node0, node1);
  }
  float approximation =
      Approximation::To<float>(node0) - Approximation::To<float>(node1);
  return approximation == 0.0f ? 0 : (approximation > 0.0f ? 1 : -1);
}

int Comparison::CompareNames(const Tree* node0, const Tree* node1) {
  int stringComparison =
      strncmp(Symbol::NonNullTerminatedName(node0),
              Symbol::NonNullTerminatedName(node1),
              std::min(Symbol::Length(node0), Symbol::Length(node1)));
  if (stringComparison == 0) {
    int delta = Symbol::Length(node0) - Symbol::Length(node1);
    return delta > 0 ? 1 : (delta == 0 ? 0 : -1);
  }
  return stringComparison;
}

int Comparison::CompareConstants(const Tree* node0, const Tree* node1) {
  return static_cast<uint8_t>(Constant::Type(node0)) -
         static_cast<uint8_t>(Constant::Type(node1));
}

int Comparison::ComparePolynomial(const Tree* node0, const Tree* node1) {
  int childrenComparison = CompareChildren(node0, node1);
  if (childrenComparison != 0) {
    return childrenComparison;
  }
  int numberOfTerms = Polynomial::NumberOfTerms(node0);
  assert(numberOfTerms == Polynomial::NumberOfTerms(node1));
  for (int i = 0; i < numberOfTerms; i++) {
    uint8_t exponent0 = Polynomial::ExponentAtIndex(node0, i);
    uint8_t exponent1 = Polynomial::ExponentAtIndex(node1, i);
    if (exponent0 != exponent1) {
      return exponent0 - exponent1;
    }
  }
  return 0;
}

int PrivateCompareChildren(const Tree* node0, const Tree* node1) {
  for (std::pair<std::array<const Tree*, 2>, int> indexedNodes :
       MultipleNodesIterator::Children<NoEditable, 2>({node0, node1})) {
    const Tree* child0 = std::get<std::array<const Tree*, 2>>(indexedNodes)[0];
    const Tree* child1 = std::get<std::array<const Tree*, 2>>(indexedNodes)[1];
    int order = Comparison::Compare(child0, child1);
    if (order != 0) {
      return order;
    }
  }
  return 0;
}

// Use a recursive method to compare the trees backward. Both number of
// nextTree() and comparison is optimal.
int CompareNextTreePairOrItself(const Tree* node0, const Tree* node1,
                                int numberOfComparisons) {
  int nextTreeComparison =
      numberOfComparisons > 1
          ? CompareNextTreePairOrItself(node0->nextTree(), node1->nextTree(),
                                        numberOfComparisons - 1)
          : 0;
  return nextTreeComparison != 0 ? nextTreeComparison
                                 : Comparison::Compare(node0, node1);
}

int PrivateCompareChildrenBackwards(const Tree* node0, const Tree* node1) {
  int numberOfChildren0 = node0->numberOfChildren();
  int numberOfChildren1 = node1->numberOfChildren();
  int numberOfComparisons = std::min(numberOfChildren0, numberOfChildren1);
  if (numberOfComparisons == 0) {
    return 0;
  }
  return CompareNextTreePairOrItself(
      node0->child(numberOfChildren0 - numberOfComparisons),
      node1->child(numberOfChildren1 - numberOfComparisons),
      numberOfComparisons);
}

int Comparison::CompareChildren(const Tree* node0, const Tree* node1,
                                bool backward) {
  int comparison = (backward ? PrivateCompareChildrenBackwards
                             : PrivateCompareChildren)(node0, node1);
  if (comparison != 0) {
    return comparison;
  }
  int numberOfChildren0 = node0->numberOfChildren();
  int numberOfChildren1 = node1->numberOfChildren();
  // The NULL node is the least node type.
  if (numberOfChildren0 < numberOfChildren1) {
    return 1;
  }
  if (numberOfChildren1 < numberOfChildren0) {
    return -1;
  }
  return 0;
}

int Comparison::CompareLastChild(const Tree* node0, const Tree* node1) {
  int m = node0->numberOfChildren();
  // Otherwise, node0 should be sanitized beforehand.
  assert(m > 0);
  int comparisonWithChild = Compare(node0->child(m - 1), node1);
  if (comparisonWithChild != 0) {
    return comparisonWithChild;
  }
  return 1;
}

bool Comparison::AreEqual(const Tree* node0, const Tree* node1) {
  // treeIsidenticalTo is faster since it uses memcmp
  bool areEqual = node0->treeIsIdenticalTo(node1);
  assert((Compare(node0, node1) == 0) == areEqual);
  return areEqual;
}

}  // namespace PoincareJ
