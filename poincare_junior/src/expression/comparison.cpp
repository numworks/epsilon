#include "approximation.h"
#include "comparison.h"
#include "constant.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include "polynomial.h"
#include "symbol.h"

namespace Poincare {

int Comparison::Compare(const Node node0, const Node node1) {
  BlockType type0 = node0.type();
  BlockType type1 = node1.type();
  TypeBlock * block0 = node0.block();
  TypeBlock * block1 = node1.block();
  if (type0 > type1) {
    // We handle the case first to implement only the upper diagonal of the comparison table
    return -Compare(node1, node0);
  } else if (type0 == type1 || (block0->isNumber() && block1->isNumber())) { // TODO: (block0->isUserNamed() && block1->isUserNamed()): do we want to group functions and symbols together?
    if (block0->isNumber()) {
      assert(block1->isNumber());
      return CompareNumbers(node0, node1);
    }
    if (block0->isUserNamed()) {
      int nameComparison = CompareNames(node0, node1);
      if (nameComparison != 0) {
        return nameComparison;
      }
      // Scan children
    }
    switch (type0) {
      case BlockType::Constant:
        return CompareConstants(node0, node1);
      case BlockType::Polynomial:
        return ComparePolynomial(node0, node1);
      case BlockType::Addition:
      case BlockType::Multiplication:
        return CompareChildren(node0, node1, ScanDirection::Backward);
      default:
        return CompareChildren(node0, node1, ScanDirection::Forward);
    }
  } else {
    assert(type0 < type1);
    switch (type0) {
      case BlockType::Addition:
      case BlockType::Multiplication:
        return CompareFirstChild(node0, node1, ScanDirection::Backward);
      case BlockType::Power:
      {
        int comparisonBase = Compare(node0.childAtIndex(0), node1);
        if (comparisonBase != 0) {
          return 1;
        }
        return Compare(node0.childAtIndex(1), &OneBlock);
      }
      case BlockType::Factorial:
        return CompareFirstChild(node0, node1, ScanDirection::Forward);
      default:
        return -1;
    }
  }
}

bool Comparison::ContainsSubtree(const Node tree, const Node subtree) {
  if (AreEqual(tree, subtree)) {
    return true;
  }
  for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(tree)) {
    Node child = std::get<Node>(indexedNode);
    if (ContainsSubtree(child, subtree)) {
      return true;
    }
  }
  return false;
}

int Comparison::CompareNumbers(const Node node0, const Node node1) {
  if (node0.block()->isRational() && node1.block()->isRational()) {
    // TODO
    //return Rational::NaturalOrder(node0, node1);
  }
  float approximation = Approximation::To<float>(node0) - Approximation::To<float>(node1);
  return approximation == 0.0f ? 0 : (approximation > 0.0f ? 1 : -1);
}

int Comparison::CompareNames(const Node node0, const Node node1) {
  int stringComparison = strncmp(Symbol::NonNullTerminatedName(node0), Symbol::NonNullTerminatedName(node1), std::min(Symbol::Length(node0), Symbol::Length(node1)) + 1);
  if (stringComparison == 0) {
    int delta = Symbol::Length(node0) - Symbol::Length(node1);
    return delta > 0 ? 1 : (delta == 0 ? 0 : -1);
  }
  return stringComparison;
}

int Comparison::CompareConstants(const Node node0, const Node node1) {
  return static_cast<uint8_t>(Constant::Type(node0)) - static_cast<uint8_t>(Constant::Type(node1));
}

int Comparison::ComparePolynomial(const Node node0, const Node node1) {
  int childrenComparison = CompareChildren(node0, node1, ScanDirection::Forward);
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

template<typename ScanDirection>
int PrivateCompareChildren(const Node node0, const Node node1) {
  for (std::pair<std::array<Node, 2>, int> indexedNodes : MultipleNodesIterator::Children<ScanDirection, NoEditable, 2>({node0, node1})) {
    Node child0 = std::get<0>(indexedNodes)[0];
    Node child1 = std::get<0>(indexedNodes)[1];
    int order = Comparison::Compare(child0, child1);
    if (order != 0) {
      return order;
    }
  }
  return 0;
}

int Comparison::CompareChildren(const Node node0, const Node node1, ScanDirection direction) {
  int comparison;
  if (direction == ScanDirection::Forward) {
    comparison = PrivateCompareChildren<Forward>(node0, node1);
  } else {
    comparison = PrivateCompareChildren<Backward>(node0, node1);
  }
  if (comparison) {
    return comparison;
  }
  int numberOfChildren0 = node0.numberOfChildren();
  int numberOfChildren1 = node1.numberOfChildren();
  // The NULL node is the least node type.
  if (numberOfChildren0 < numberOfChildren1) {
    return 1;
  }
  if (numberOfChildren1 < numberOfChildren0) {
    return -1;
  }
  return 0;
}

int Comparison::CompareFirstChild(const Node node0, Node node1, ScanDirection direction) {
  uint8_t indexOfChild = direction == ScanDirection::Forward ? 0 : node0.numberOfChildren() - 1;
  int comparisonWithChild = Compare(node0.childAtIndex(indexOfChild), node1);
  if (comparisonWithChild != 0) {
    return comparisonWithChild;
  }
  assert(node0.numberOfChildren() > 1);
  return 1;
}

}
