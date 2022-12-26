#include "comparison.h"
#include "integer.h"
#include "polynomial.h"
#include "set.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <poincare_junior/src/n_ary.h>

namespace Poincare {

uint8_t Polynomial::ExponantAtIndex(const Node polynomial, int index) {
  assert(index >= 0 && index < polynomial.numberOfChildren());
  return static_cast<uint8_t>(*(polynomial.block()->nextNth(2 + index)));
}

void Polynomial::Add(EditionReference polynomial, std::pair<EditionReference, uint8_t> monomial) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<EditionReference>(monomial);
  int nbOfTerms = polynomial.numberOfChildren();
  for (int i = 0; i <= nbOfTerms; i++) {
    int16_t exponentOfChildI = i < nbOfTerms ? ExponantAtIndex(polynomial, i) : -1;
    if (exponent < exponentOfChildI) {
      continue;
    } else if (exponent == exponentOfChildI) {
      EditionReference currentCoefficient = polynomial.childAtIndex(i);
      if (currentCoefficient.type() == BlockType::Multiplication) {
        NAry::AddChild(currentCoefficient, coefficient);
      } else {
        currentCoefficient.insertNodeBeforeNode(EditionReference::Push<BlockType::Multiplication>(2));
        currentCoefficient.nextTree().insertTreeBeforeNode(coefficient);
      }
    } else {
      NAry::AddChildAtIndex(polynomial, coefficient, i);
      Block * exponentsAddress = polynomial.block() + 2;
      pool->insertBlock(exponentsAddress + i, ValueBlock(exponent));
    }
    break;
  }
}

EditionReference PolynomialParser::GetVariables(const Node expression) {
  if (expression.block()->isInteger()) { // TODO: generic belongToField?
    return EditionReference(Set());
  }
  BlockType type = expression.type();
  // TODO: match
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponent = base.nextTree();
    if (Integer::IsUint8(exponent)) {
      assert(Integer::Uint8(exponent) > 1);
      EditionReference variables = EditionReference::Push<BlockType::Set>(1);
      EditionReference::Clone(base);
      return variables;
    }
  }
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    EditionReference variables = EditionReference(Set());
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      if (child.type() == BlockType::Addition) {
        assert(type != BlockType::Addition);
        variables = Set::Add(variables, child);
      } else {
        variables = Set::Union(variables, GetVariables(child));
      }
    }
    return variables;
  }
  EditionReference variables = EditionReference::Push<BlockType::Set>(1);
  EditionReference::Clone(expression);
  return variables;
}

EditionReference PolynomialParser::RecursivelyParse(EditionReference expression, EditionReference variables, size_t variableIndex) {
  EditionReference variable;
  for (std::pair<Node, int> indexedVariable : NodeIterator::Children<Forward, NoEditable>(variables)) {
    if (std::get<int>(indexedVariable) < variableIndex) {
      // Skip previously handled variable
      continue;
    }
    if (Comparison::ContainsSubtree(expression, std::get<Node>(indexedVariable))) {
      variable = std::get<Node>(indexedVariable);
      break;
    }
  }
  if (variable.isUninitialized()) {
    // expression is not a polynomial of variables
    return expression;
  }
  expression = Parse(expression, variable);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(expression)) {
    EditionReference child = std::get<EditionReference>(indexedRef);
    RecursivelyParse(child, variables, variableIndex + 1);
  }
  return expression;
}

EditionReference PolynomialParser::Parse(EditionReference expression, EditionReference variable) {
  EditionReference polynomial(Pol());
  BlockType type = expression.type();
  if (type == BlockType::Addition) {
    for (size_t i = 0 ; i < expression.numberOfChildren(); i++) {
      /* We deplete the addition from its children as we scan it so we can
       * always take the first child. */
      EditionReference child = expression.nextNode();
      Polynomial::Add(polynomial, ParseMonomial(child, variable));
    }
    // Addition node has been emptied from children
    expression.replaceNodeByTree(polynomial);
  } else {
    // Insert polynomial next to expression before it's parsed (and likely replaced)
    expression.insertNodeBeforeNode(polynomial);
    Polynomial::Add(polynomial, ParseMonomial(expression, variable));
  }
  return polynomial;
}


std::pair<EditionReference, uint8_t> PolynomialParser::ParseMonomial(EditionReference expression, EditionReference variable) {
  if (Comparison::AreEqual(expression, variable)) {
    expression.replaceTreeByTree(Node(&OneBlock));
    return std::make_pair(expression, 1);
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponent = base.nextTree();
    if (Comparison::AreEqual(base, variable) && Integer::IsUint8(exponent)) {
      uint8_t exp = Integer::Uint8(exponent);
      assert(exp > 1);
      expression.replaceTreeByTree(Node(&OneBlock));
      return std::make_pair(expression, exp);
    }
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(expression)) {
      EditionReference child = std::get<EditionReference>(indexedRef);
      auto [childCoefficient, childExponant] = ParseMonomial(EditionReference::Clone(child), variable);
      if (childExponant > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        /* TODO: if the previous assertion is wrong, we have to multiply
         * children coefficients and addition children exponents. */
        child.replaceTreeByTree(childCoefficient);
        // TODO: call basicReduction
        return std::make_pair(expression, childExponant);
      }
      childCoefficient.removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(!Comparison::ContainsSubtree(expression, variable));
  return std::make_pair(expression, 0);
}

#if 0
bool IsInSetOrIsEqual(const Node expression, const Node variables) {
  return variables.type() == BlockType::Set ?
    Set::Includes(variables, expression) :
    Compare::AreEqual(variables, expression);
}

uint8_t Polynomial::Degree(const Node expression, const Node variable) {
  if (Compare::AreEqual(expression, variable)) {
    return 1;
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponent = base.nextTree();
    if (Integer::IsUint8(exponent) && Compare::AreEqual(base, variable)) {
      return Integer::Uint8(exponent);
    }
  }
  uint8_t degree = 0;
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      uint8_t childDegree = Degree(child, variables);
      if (type == BlockType::Addition) {
        degree = std::max(degree, childDegree);
      } else {
        degree += childDegree;
      }
    }
  }
  // TODO assert(!Number::IsZero(expression));
  return degree;
}

EditionReference Polynomial::Coefficient(const Node expression, const Node variable, uint8_t exponent) {
  BlockType type = expression.type();
  if (expression.type() == BlockType::Addition) {
    if (Comparison::AreEqual(expression, variable)) {
      return exponent == 1 ? EditionReference::Push<BlockType::One>() : EditionReference::Push<BlockType::Zero>();
    }
    EditionReference addition = EditionReference::Push<BlockType::Addition>(0);
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      auto [childCoefficient, childExponant] = MonomialCoefficient(child, variable);
      if (childExponant == exponent) {
        NAry::AddChild(addition, childCoefficient);
      } else {
        childCoefficient.removeTree();
      }
    }
    return addition; // TODO: apply basicReduction
  }
  auto [exprCoefficient, exprExponant] = MonomialCoefficient(expression, variable);
  if (exponent == exprExponant) {
    return exprCoefficient;
  }
  exprCoefficient.removeTree();
  return EditionReference::Push<BlockType::Zero>();
}

std::pair<EditionReference, uint8_t> Polynomial::MonomialCoefficient(const Node expression, const Node variable) {
  if (Comparison::AreEqual(expression, variable)) {
    return std::make_pair(EditionReference::Push<BlockType::One>(), 1);
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Node base = expression.nextNode();
    Node exponent = base.nextTree();
    if (Comparison::AreEqual(exponent, variable) && Integer::IsUint8(exponent)) {
      assert(Integer::Uint8(exponent) > 1);
      return std::make_pair(EditionReference::Push<BlockType::One>(), Integer::Uint8(exponent));
    }
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(expression)) {
      Node child = std::get<Node>(indexedNode);
      auto [childCoefficient, childExponant] = MonomialCoefficient(child, variable);
      if (childExponant > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        EditionReference multCopy = EditionReference::Clone(expression);
        multCopy.childAtIndex(std::get<int>(indexedNode)).replaceTreeByTree(childCoefficient);
        return std::make_pair(multCopy, childExponant);
      }
      childCoefficient.removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(Comparison::ContainsSubtree(expression, variable));
  return std::make_pair(EditionReference::Clone(expression), 0);
}

#endif
}
