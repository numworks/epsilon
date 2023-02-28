#include "comparison.h"
#include "polynomial.h"
#include "rational.h"
#include "set.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/value_block.h>
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

/* Polynomial */

EditionReference Polynomial::PushEmpty(EditionReference variable) {
  EditionReference pol = EditionReference::Push<BlockType::Polynomial>(1, 1);
  pol.insertTreeAfterNode(variable);
  return pol;
}

EditionReference Polynomial::PushMonomial(EditionReference variable, uint8_t exponent, EditionReference coefficient) {
  EditionReference pol = PushEmpty(variable);
  AddMonomial(pol, std::make_pair(coefficient, exponent));
  return pol;
}

uint8_t Polynomial::ExponentAtIndex(const Node polynomial, int index) {
  assert(index >= 0 && index < NumberOfTerms(polynomial));
  return static_cast<uint8_t>(*(polynomial.block()->nextNth(2 + index)));
}

void Polynomial::SetExponentAtIndex(EditionReference polynomial, int index, uint8_t exponent) {
  Block * exponentsAddress = polynomial.block() + 2;
  *(exponentsAddress + index) = exponent;
}

void Polynomial::InsertExponentAtIndex(EditionReference polynomial, int index, uint8_t exponent) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Block * exponentsAddress = polynomial.block() + 2;
  pool->insertBlock(exponentsAddress + index, ValueBlock(exponent));
}

void Polynomial::RemoveExponentAtIndex(EditionReference polynomial, int index) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Block * exponentsAddress = polynomial.block() + 2;
  pool->removeBlocks(exponentsAddress + index, 1);
}

void Polynomial::AddMonomial(EditionReference polynomial, std::pair<EditionReference, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<EditionReference>(monomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  for (int i = 0; i <= nbOfTerms; i++) {
    int16_t exponentOfChildI = i < nbOfTerms ? ExponentAtIndex(polynomial, i) : -1;
    if (exponent < exponentOfChildI) {
      continue;
    } else if (exponent == exponentOfChildI) {
      EditionReference currentCoefficient = polynomial.childAtIndex(i + 1);
      EditionReference previousChild = currentCoefficient.previousTree();
      EditionReference addition = Polynomial::Addition(currentCoefficient, coefficient);
      previousChild.nextTree().insertTreeBeforeNode(addition);
    } else {
      NAry::AddChildAtIndex(polynomial, coefficient, i + 1);
      InsertExponentAtIndex(polynomial, i, exponent);
    }
    break;
  }
}

EditionReference Polynomial::Addition(EditionReference polA, EditionReference polB) {
  return Operation(
      polA,
      polB,
      BlockType::Addition,
      AddMonomial,
      [](EditionReference result, EditionReference polynomial, std::pair<EditionReference, uint8_t> monomial, bool isLastTerm) {
        AddMonomial(polynomial, monomial);
        result.replaceTreeByTree(polynomial);
        return polynomial;
      });
}

EditionReference Polynomial::Multiplication(EditionReference polA, EditionReference polB) {
  return Operation(
      polA,
      polB,
      BlockType::Multiplication,
      MultiplicationMonomial,
      [](EditionReference result, EditionReference polynomial, std::pair<EditionReference, uint8_t> monomial, bool isLastTerm) {
        EditionReference polynomialClone = isLastTerm ? polynomial : EditionReference::Clone(polynomial);
        MultiplicationMonomial(polynomialClone, monomial);
        return Addition(result, polynomialClone);
      });
}

EditionReference Polynomial::Subtraction(EditionReference polA, EditionReference polB) {
  return Addition(polA, Multiplication(polB, EditionReference(&MinusOneBlock)));
}

EditionReference Polynomial::Operation(EditionReference polA, EditionReference polB, BlockType blockType, OperationMonomial operationMonomial, OperationReduce operationMonomialAndReduce) {
  if (polA.type() != BlockType::Polynomial) {
    if (polB.type() != BlockType::Polynomial) {
      EditionReference op = blockType == BlockType::Addition ? EditionReference::Push<BlockType::Addition>(2) : EditionReference::Push<BlockType::Multiplication>(2);
      op.insertTreeAfterNode(polB);
      op.insertTreeAfterNode(polA);
      return op; // TODO: basicReduction
    }
    return Operation(polB, polA, blockType, operationMonomial, operationMonomialAndReduce);
  }
  EditionReference x = Variable(polA);
  if (polB.type() == BlockType::Polynomial && Comparison::Compare(x, Variable(polB)) > 0) {
    return Operation(polB, polA, blockType, operationMonomial, operationMonomialAndReduce);
  }
  if (polB.type() != BlockType::Polynomial || !Comparison::AreEqual(x, Variable(polB))) {
    operationMonomial(polA, std::make_pair(polB, static_cast<uint8_t>(0)));
  } else {
    // Both polA and polB are polynom(x)
    EditionReference variableB = polB.nextNode();
    EditionReference coefficientB = variableB.nextTree();
    size_t i = 0;
    uint8_t nbOfTermsB = NumberOfTerms(polB);
    variableB.removeTree();
    EditionReference result(&ZeroBlock);
    while (i < nbOfTermsB) {
      EditionReference nextCoefficientB = coefficientB.nextTree();
      result = operationMonomialAndReduce(result, polA, std::make_pair(coefficientB, ExponentAtIndex(polB, i)), i == nbOfTermsB - 1);
      coefficientB = nextCoefficientB;
      i++;
    }
    // polB children have been pilfered; remove the node and the variable child
    polB.removeNode();
    // polA has been merged in result
    polA = result;
  }
  return Sanitize(polA);
}

void Polynomial::MultiplicationMonomial(EditionReference polynomial, std::pair<EditionReference, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<EditionReference>(monomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  for (int i = 0; i < nbOfTerms; i++) {
    // * x^exponent
    SetExponentAtIndex(polynomial, i, ExponentAtIndex(polynomial, i) + exponent);
    // * coefficient
    EditionReference currentCoefficient = polynomial.childAtIndex(i + 1);
    EditionReference previousChild = currentCoefficient.previousTree();
    // Avoid one cloning for last term
    EditionReference coeffClone = i == nbOfTerms - 1 ? coefficient : EditionReference::Clone(coefficient);
    EditionReference multiplication = Polynomial::Multiplication(currentCoefficient, coeffClone);
    previousChild.nextTree().insertTreeBeforeNode(multiplication);
  }
}

static void extractDegreeAndLeadingCoefficient(EditionReference pol, EditionReference x,  uint8_t * degree, EditionReference * coefficient) {
  if (Comparison::AreEqual(x, Polynomial::Variable(pol))) {
    *degree = Polynomial::Degree(pol);
    *coefficient = Polynomial::LeadingCoefficient(pol);
  } else {
    *degree = 0;
    *coefficient = pol;
  }
}

std::pair<EditionReference, EditionReference> Polynomial::PseudoDivision(EditionReference polA, EditionReference polB) {
  if (polA.type() != BlockType::Polynomial && polB.type() != BlockType::Polynomial) {
    assert(polA.block()->isInteger() && polB.block()->isInteger());
    auto [quotient, remainder] = IntegerHandler::Division(Integer::Handler(polA), Integer::Handler(polB));
    polB.removeTree();
    if (remainder.type() == BlockType::Zero) {
      polA.removeTree();
      return std::make_pair(quotient, remainder);
    }
    quotient.removeTree();
    remainder.removeTree();
    return std::make_pair(EditionReference(&ZeroBlock), polA);
  }
  if (polA.type() != BlockType::Polynomial) {
    polB.removeTree();
    return std::make_pair(EditionReference(&ZeroBlock), polA);
  }
  EditionReference x = Variable(polA);
  if (polB.type() == BlockType::Polynomial && Comparison::Compare(x, Variable(polB)) > 0) {
    x = Variable(polB);
  }
  uint8_t degreeA, degreeB;
  EditionReference leadingCoeffA, leadingCoeffB;
  extractDegreeAndLeadingCoefficient(polA, x, &degreeA, &leadingCoeffA);
  extractDegreeAndLeadingCoefficient(polB, x, &degreeB, &leadingCoeffB);
  EditionReference currentQuotient(&ZeroBlock);
  while (degreeA >= degreeB) {
    auto [quotient, remainder] = PseudoDivision(EditionReference::Clone(leadingCoeffA), EditionReference::Clone(leadingCoeffB));
    bool stopCondition = remainder.type() != BlockType::Zero;
    remainder.removeTree();
    if (stopCondition) {
      quotient.removeTree();
      break;
    }
    EditionReference xPowerDegAMinusDegB = Polynomial::PushMonomial(EditionReference::Clone(x), degreeA - degreeB);
    currentQuotient = Polynomial::Addition(currentQuotient, Polynomial::Multiplication(EditionReference::Clone(quotient), EditionReference::Clone(xPowerDegAMinusDegB)));
    polA = Polynomial::Subtraction(polA, Polynomial::Multiplication(quotient, Polynomial::Multiplication(EditionReference::Clone(polB), xPowerDegAMinusDegB)));
    extractDegreeAndLeadingCoefficient(polA, x, &degreeA, &leadingCoeffA);
  }
  polB.removeTree();
  return std::make_pair(currentQuotient, polA);
}

EditionReference Polynomial::Sanitize(EditionReference polynomial) {
  uint8_t nbOfTerms = NumberOfTerms(polynomial);
  size_t i = 0;
  EditionReference coefficient = polynomial.childAtIndex(1);
  while (i < nbOfTerms) {
    EditionReference nextCoefficient = coefficient.nextTree();
    if (coefficient.type() == BlockType::Zero) {
      coefficient.removeTree();
      NAry::SetNumberOfChildren(polynomial, polynomial.numberOfChildren() - 1);
      RemoveExponentAtIndex(polynomial, i);
    }
    coefficient = nextCoefficient;
    i++;
  }
  if (polynomial.numberOfChildren() == 1) {
    EditionReference result = EditionReference(&ZeroBlock);
    polynomial.replaceTreeByTree(result);
    return result;
  }
  if (polynomial.numberOfChildren() == 2 && ExponentAtIndex(polynomial, 0) == 0) {
    EditionReference result = polynomial.childAtIndex(1);
    polynomial.replaceTreeByTree(result);
    return result;
  }
  return polynomial;
}

/* PolynomialParser */

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
    if (std::get<int>(indexedRef) == 0) {
      // Pass variable child
      continue;
    }
    EditionReference child = std::get<EditionReference>(indexedRef);
    RecursivelyParse(child, variables, variableIndex + 1);
  }
  return expression;
}

EditionReference PolynomialParser::Parse(EditionReference expression, EditionReference variable) {
  EditionReference polynomial = Polynomial::PushEmpty(EditionReference::Clone(variable));
  BlockType type = expression.type();
  if (type == BlockType::Addition) {
    for (size_t i = 0 ; i < expression.numberOfChildren(); i++) {
      /* We deplete the addition from its children as we scan it so we can
       * always take the first child. */
      EditionReference child = expression.nextNode();
      Polynomial::AddMonomial(polynomial, ParseMonomial(child, variable));
    }
    polynomial = Polynomial::Sanitize(polynomial);
    // Addition node has been emptied from children
    expression.replaceNodeByTree(polynomial);
  } else {
    // Insert polynomial next to expression before it's parsed (and likely replaced)
    expression.insertTreeBeforeNode(polynomial);
    Polynomial::AddMonomial(polynomial, ParseMonomial(expression, variable));
  }
  return polynomial;
}

std::pair<EditionReference, uint8_t> PolynomialParser::ParseMonomial(EditionReference expression, EditionReference variable) {
  if (Comparison::AreEqual(expression, variable)) {
    expression.replaceTreeByTree(Node(&OneBlock));
    return std::make_pair(expression, static_cast<uint8_t>(1));
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
      auto [childCoefficient, childExponent] = ParseMonomial(EditionReference::Clone(child), variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        /* TODO: if the previous assertion is wrong, we have to multiply
         * children coefficients and addition children exponents. */
        child.replaceTreeByTree(childCoefficient);
        // TODO: call basicReduction
        return std::make_pair(expression, childExponent);
      }
      childCoefficient.removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(!Comparison::ContainsSubtree(expression, variable));
  return std::make_pair(expression, static_cast<uint8_t>(0));
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
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent == exponent) {
        NAry::AddChild(addition, childCoefficient);
      } else {
        childCoefficient.removeTree();
      }
    }
    return addition; // TODO: apply basicReduction
  }
  auto [exprCoefficient, exprExponent] = MonomialCoefficient(expression, variable);
  if (exponent == exprExponent) {
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
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        EditionReference multCopy = EditionReference::Clone(expression);
        multCopy.childAtIndex(std::get<int>(indexedNode)).replaceTreeByTree(childCoefficient);
        return std::make_pair(multCopy, childExponent);
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
