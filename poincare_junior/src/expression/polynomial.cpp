#include "polynomial.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/value_block.h>
#include <poincare_junior/src/n_ary.h>

#include "comparison.h"
#include "rational.h"
#include "set.h"
#include "simplification.h"

namespace PoincareJ {

/* Polynomial */

Tree* Polynomial::PushEmpty(const Tree* variable) {
  Tree* pol(SharedEditionPool->push<BlockType::Polynomial>(1, 1));
  pol->cloneTreeAfterNode(variable);
  return pol;
}

Tree* Polynomial::PushMonomial(const Tree* variable, uint8_t exponent,
                               const Tree* coefficient) {
  Tree* pol = PushEmpty(variable);
  AddMonomial(pol, std::make_pair(coefficient->clone(), exponent));
  return pol;
}

uint8_t Polynomial::ExponentAtIndex(const Tree* polynomial, int index) {
  assert(index >= 0 && index < NumberOfTerms(polynomial));
  return static_cast<uint8_t>(*(polynomial->block()->nextNth(2 + index)));
}

void Polynomial::SetExponentAtIndex(Tree* polynomial, int index,
                                    uint8_t exponent) {
  Block* exponentsAddress = polynomial->block() + 2;
  *(exponentsAddress + index) = exponent;
}

void Polynomial::InsertExponentAtIndex(Tree* polynomial, int index,
                                       uint8_t exponent) {
  Block* exponentsAddress = polynomial->block() + 2;
  SharedEditionPool->insertBlock(exponentsAddress + index, ValueBlock(exponent),
                                 true);
}

void Polynomial::RemoveExponentAtIndex(Tree* polynomial, int index) {
  Block* exponentsAddress = polynomial->block() + 2;
  SharedEditionPool->removeBlocks(exponentsAddress + index, 1);
}

void Polynomial::AddMonomial(EditionReference polynomial,
                             std::pair<EditionReference, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<EditionReference>(monomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  for (int i = 0; i <= nbOfTerms; i++) {
    int16_t exponentOfChildI =
        i < nbOfTerms ? ExponentAtIndex(polynomial, i) : -1;
    if (exponent < exponentOfChildI) {
      continue;
    } else if (exponent == exponentOfChildI) {
      EditionReference previousChild = polynomial->childAtIndex(i);
      EditionReference currentCoefficient = previousChild->nextTree();
      EditionReference addition =
          Polynomial::Addition(currentCoefficient, coefficient);
      previousChild->nextTree()->moveTreeBeforeNode(addition);
    } else {
      NAry::AddChildAtIndex(polynomial, coefficient, i + 1);
      InsertExponentAtIndex(polynomial, i, exponent);
    }
    break;
  }
}

EditionReference Polynomial::Addition(EditionReference polA,
                                      EditionReference polB) {
  return Operation(
      polA, polB, BlockType::Addition, AddMonomial,
      [](EditionReference result, EditionReference polynomial,
         std::pair<EditionReference, uint8_t> monomial, bool isLastTerm) {
        AddMonomial(polynomial, monomial);
        result->moveTreeOverTree(polynomial);
        return polynomial;
      });
}

EditionReference Polynomial::Multiplication(EditionReference polA,
                                            EditionReference polB) {
  // TODO: implement Kronecker-Shönhage trick?
  return Operation(
      polA, polB, BlockType::Multiplication, MultiplicationMonomial,
      [](EditionReference result, EditionReference polynomial,
         std::pair<EditionReference, uint8_t> monomial, bool isLastTerm) {
        EditionReference polynomialClone =
            isLastTerm ? polynomial
                       : EditionReference(SharedEditionPool->clone(polynomial));
        MultiplicationMonomial(polynomialClone, monomial);
        return Addition(result, polynomialClone);
      });
}

EditionReference Polynomial::Subtraction(EditionReference polA,
                                         EditionReference polB) {
  return Addition(polA, Multiplication(polB, EditionReference(-1_e)));
}

EditionReference Polynomial::Operation(
    EditionReference polA, EditionReference polB, BlockType blockType,
    OperationMonomial operationMonomial,
    OperationReduce operationMonomialAndReduce) {
  if (polA->type() != BlockType::Polynomial) {
    if (polB->type() != BlockType::Polynomial) {
      EditionReference op =
          blockType == BlockType::Addition
              ? SharedEditionPool->push<BlockType::Addition>(2)
              : SharedEditionPool->push<BlockType::Multiplication>(2);
      op->moveTreeAfterNode(polB);
      op->moveTreeAfterNode(polA);
      Simplification::SystematicReduce(op);
      return op;
    }
    return Operation(polB, polA, blockType, operationMonomial,
                     operationMonomialAndReduce);
  }
  EditionReference x = Variable(polA);
  if (polB->type() == BlockType::Polynomial &&
      Comparison::Compare(x, Variable(polB)) > 0) {
    return Operation(polB, polA, blockType, operationMonomial,
                     operationMonomialAndReduce);
  }
  if (polB->type() != BlockType::Polynomial ||
      !Comparison::AreEqual(x, Variable(polB))) {
    operationMonomial(polA, std::make_pair(polB, static_cast<uint8_t>(0)));
  } else {
    // Both polA and polB are polynom(x)
    EditionReference variableB = polB->nextNode();
    EditionReference coefficientB = variableB->nextTree();
    size_t i = 0;
    uint8_t nbOfTermsB = NumberOfTerms(polB);
    variableB->removeTree();
    EditionReference result(0_e);
    assert(i < nbOfTermsB);
    while (i < nbOfTermsB) {
      EditionReference nextCoefficientB = coefficientB->nextTree();
      result = operationMonomialAndReduce(
          result, polA, std::make_pair(coefficientB, ExponentAtIndex(polB, i)),
          i == nbOfTermsB - 1);
      coefficientB = nextCoefficientB;
      i++;
    }
    // polB children have been pilfered; remove the node and the variable child
    polB->removeNode();
    // polA has been merged in result
    polA = result;
  }
  return Sanitize(polA);
}

void Polynomial::MultiplicationMonomial(
    EditionReference polynomial,
    std::pair<EditionReference, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<EditionReference>(monomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  assert(0 < nbOfTerms);
  for (int i = 0; i < nbOfTerms; i++) {
    // * x^exponent
    SetExponentAtIndex(polynomial, i,
                       ExponentAtIndex(polynomial, i) + exponent);
    // * coefficient
    EditionReference previousChild = polynomial->childAtIndex(i);
    EditionReference currentCoefficient = previousChild->nextTree();
    // Avoid one cloning for last term
    EditionReference coeffClone =
        i == nbOfTerms - 1
            ? coefficient
            : EditionReference(SharedEditionPool->clone(coefficient));
    EditionReference multiplication =
        Polynomial::Multiplication(currentCoefficient, coeffClone);
    previousChild->nextTree()->moveTreeBeforeNode(multiplication);
  }
}

static void extractDegreeAndLeadingCoefficient(EditionReference pol,
                                               EditionReference x,
                                               uint8_t* degree,
                                               EditionReference* coefficient) {
  if (pol->type() == BlockType::Polynomial &&
      Comparison::AreEqual(x, Polynomial::Variable(pol))) {
    *degree = Polynomial::Degree(pol);
    *coefficient = Polynomial::LeadingCoefficient(pol);
  } else {
    *degree = 0;
    *coefficient = pol;
  }
}

std::pair<EditionReference, EditionReference> Polynomial::PseudoDivision(
    EditionReference polA, EditionReference polB) {
  if (polA->type() != BlockType::Polynomial &&
      polB->type() != BlockType::Polynomial) {
    assert(polA->block()->isInteger() && polB->block()->isInteger());
    std::pair<Tree*, Tree*> nodePair = IntegerHandler::Division(
        Integer::Handler(polA), Integer::Handler(polB));
    EditionReference quotient = EditionReference(nodePair.first);
    EditionReference remainder = EditionReference(nodePair.second);
    polB->removeTree();
    if (remainder->type() == BlockType::Zero) {
      polA->removeTree();
      return std::make_pair(quotient, remainder);
    }
    quotient->removeTree();
    remainder->removeTree();
    return std::make_pair(EditionReference(0_e), polA);
  }
  if (polA->type() != BlockType::Polynomial) {
    polB->removeTree();
    return std::make_pair(EditionReference(0_e), polA);
  }
  bool isXCloned = false;
  EditionReference x = Variable(polA);
  if (polB->type() == BlockType::Polynomial &&
      Comparison::Compare(x, Variable(polB)) >= 0) {
    x = Variable(polB);
  } else {
    // Clone x since polA may be altered
    x = SharedEditionPool->clone(x);
    isXCloned = true;
  }
  uint8_t degreeA, degreeB;
  EditionReference leadingCoeffA, leadingCoeffB;
  extractDegreeAndLeadingCoefficient(polA, x, &degreeA, &leadingCoeffA);
  extractDegreeAndLeadingCoefficient(polB, x, &degreeB, &leadingCoeffB);
  EditionReference currentQuotient(0_e);
  while (degreeA >= degreeB) {
    std::pair<EditionReference, EditionReference> refPair =
        PseudoDivision(SharedEditionPool->clone(leadingCoeffA),
                       SharedEditionPool->clone(leadingCoeffB));
    EditionReference quotient = refPair.first;
    EditionReference remainder = refPair.second;
    bool stopCondition = remainder->type() != BlockType::Zero;
    remainder->removeTree();
    if (stopCondition) {
      quotient->removeTree();
      break;
    }
    EditionReference xPowerDegAMinusDegB = Polynomial::PushMonomial(
        SharedEditionPool->clone(x), degreeA - degreeB);
    currentQuotient = Polynomial::Addition(
        currentQuotient, Polynomial::Multiplication(
                             SharedEditionPool->clone(quotient),
                             SharedEditionPool->clone(xPowerDegAMinusDegB)));
    polA = Polynomial::Subtraction(
        polA,
        Polynomial::Multiplication(
            quotient, Polynomial::Multiplication(SharedEditionPool->clone(polB),
                                                 xPowerDegAMinusDegB)));
    extractDegreeAndLeadingCoefficient(polA, x, &degreeA, &leadingCoeffA);
  }
  polB->removeTree();
  if (isXCloned) {
    x->removeTree();
  }
  return std::make_pair(currentQuotient, polA);
}

EditionReference Polynomial::Sanitize(EditionReference polynomial) {
  uint8_t nbOfTerms = NumberOfTerms(polynomial);
  size_t i = 0;
  EditionReference coefficient = polynomial->childAtIndex(1);
  while (i < nbOfTerms) {
    EditionReference nextCoefficient = coefficient->nextTree();
    if (coefficient->type() == BlockType::Zero) {
      coefficient->removeTree();
      NAry::SetNumberOfChildren(polynomial, polynomial->numberOfChildren() - 1);
      RemoveExponentAtIndex(polynomial, i);
    }
    coefficient = nextCoefficient;
    i++;
  }
  int numberOfTerms = NumberOfTerms(polynomial);
  if (numberOfTerms == 0) {
    return EditionReference(polynomial->cloneTreeOverTree(0_e));
  }
  if (numberOfTerms == 1 && ExponentAtIndex(polynomial, 0) == 0) {
    EditionReference result = polynomial->childAtIndex(1);
    polynomial->moveTreeOverTree(result);
    return result;
  }
  // Assert the exponents are ordered
  for (int i = 1; i < numberOfTerms; i++) {
    assert(ExponentAtIndex(polynomial, i - 1) > ExponentAtIndex(polynomial, i));
  }
  return polynomial;
}

/* PolynomialParser */

const Tree* PolynomialParser::GetVariables(const Tree* expression) {
  if (expression->block()->isInteger()) {  // TODO: generic belongToField?
    return KSet();
  }
  BlockType type = expression->type();
  // TODO: match
  if (type == BlockType::Power) {
    const Tree* base = expression->nextNode();
    const Tree* exponent = base->nextTree();
    if (Integer::IsUint8(exponent)) {
      assert(Integer::Uint8(exponent) > 1);
      EditionReference variables(SharedEditionPool->push<BlockType::Set>(1));
      SharedEditionPool->clone(base);
      return variables;
    }
  }
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    EditionReference variables = EditionReference(KSet());
    for (const Tree* child : expression->children()) {
      if (child->type() == BlockType::Addition) {
        assert(type != BlockType::Addition);
        variables = Set::Add(variables, child);
      } else {
        variables = Set::Union(variables, GetVariables(child));
      }
    }
    return variables;
  }
  Tree* variables = SharedEditionPool->push<BlockType::Set>(1);
  SharedEditionPool->clone(expression);
  return variables;
}

EditionReference PolynomialParser::RecursivelyParse(EditionReference expression,
                                                    EditionReference variables,
                                                    size_t variableIndex) {
  const Tree* variable = nullptr;
  for (std::pair<const Tree*, int> indexedVariable :
       NodeIterator::Children<NoEditable>(variables)) {
    if (std::get<int>(indexedVariable) < variableIndex) {
      // Skip previously handled variable
      continue;
    }
    if (Comparison::ContainsSubtree(expression,
                                    std::get<const Tree*>(indexedVariable))) {
      variable = std::get<const Tree*>(indexedVariable);
      break;
    }
  }
  if (!variable) {
    // expression is not a polynomial of variables
    return expression;
  }
  expression = Parse(expression, variable);
  for (std::pair<EditionReference, int> indexedRef :
       NodeIterator::Children<Editable>(expression)) {
    if (std::get<int>(indexedRef) == 0) {
      // Pass variable child
      continue;
    }
    EditionReference child = std::get<EditionReference>(indexedRef);
    RecursivelyParse(child, variables, variableIndex + 1);
  }
  return expression;
}

EditionReference PolynomialParser::Parse(EditionReference expression,
                                         EditionReference variable) {
  EditionReference polynomial =
      Polynomial::PushEmpty(SharedEditionPool->clone(variable));
  BlockType type = expression->type();
  if (type == BlockType::Addition) {
    for (size_t i = 0; i < expression->numberOfChildren(); i++) {
      /* We deplete the addition from its children as we scan it so we can
       * always take the first child. */
      EditionReference child = expression->nextNode();
      Polynomial::AddMonomial(polynomial, ParseMonomial(child, variable));
    }
    polynomial = Polynomial::Sanitize(polynomial);
    // Addition node has been emptied from children
    expression->moveTreeOverNode(polynomial);
  } else {
    // Move polynomial next to expression before it's parsed (and likely
    // replaced)
    expression->moveTreeBeforeNode(polynomial);
    Polynomial::AddMonomial(polynomial, ParseMonomial(expression, variable));
  }
  return polynomial;
}

std::pair<EditionReference, uint8_t> PolynomialParser::ParseMonomial(
    EditionReference expression, EditionReference variable) {
  if (Comparison::AreEqual(expression, variable)) {
    return std::make_pair(EditionReference(expression->cloneTreeOverTree(1_e)),
                          static_cast<uint8_t>(1));
  }
  PatternMatching::Context ctx;
  ctx.setNode(Placeholders::A, variable, 1, false);
  if (PatternMatching::Match(KPow(KPlaceholder<Placeholders::A>(),
                                  KPlaceholder<Placeholders::B>()),
                             expression, &ctx)) {
    const Tree* exponent = ctx.getNode(Placeholders::B);
    if (Integer::IsUint8(exponent)) {
      uint8_t exp = Integer::Uint8(exponent);
      assert(exp > 1);
      return std::make_pair(
          EditionReference(expression->cloneTreeOverTree(1_e)), exp);
    }
  }
  if (expression->type() == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedRef :
         NodeIterator::Children<Editable>(expression)) {
      EditionReference child = std::get<EditionReference>(indexedRef);
      auto [childCoefficient, childExponent] =
          ParseMonomial(SharedEditionPool->clone(child), variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at
        // basicReduction
        /* TODO: if the previous assertion is wrong, we have to multiply
         * children coefficients and addition children exponents. */
        child->moveTreeOverTree(childCoefficient);
        Simplification::SystematicReduce(expression);
        return std::make_pair(expression, childExponent);
      }
      childCoefficient->removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(!Comparison::ContainsSubtree(expression, variable));
  return std::make_pair(expression, static_cast<uint8_t>(0));
}

#if 0
bool IsInSetOrIsEqual(const Tree* expression, const Tree* variables) {
  return variables.type() == BlockType::Set ?
    Set::Includes(variables, expression) :
    Compare::AreEqual(variables, expression);
}

uint8_t Polynomial::Degree(const Tree* expression, const Tree* variable) {
  if (Compare::AreEqual(expression, variable)) {
    return 1;
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Tree* base = expression.nextNode();
    Tree* exponent = base.nextTree();
    if (Integer::IsUint8(exponent) && Compare::AreEqual(base, variable)) {
      return Integer::Uint8(exponent);
    }
  }
  uint8_t degree = 0;
  if (type == BlockType::Addition || type == BlockType::Multiplication) {
    for (const Tree* child : expression->children()) {
      uint8_t childDegree = Degree(child, variables);
      if (type == BlockType::Addition) {
        degree = std::max(degree, childDegree);
      } else {
        degree += childDegree;
      }
    }
  }
  assert(!Number::IsZero(expression));
  return degree;
}

EditionReference Polynomial::Coefficient(const Tree* expression, const Tree* variable, uint8_t exponent) {
  BlockType type = expression.type();
  if (expression.type() == BlockType::Addition) {
    EditionPool * SharedEditionPool = SharedEditionPool;
    if (Comparison::AreEqual(expression, variable)) {
      return exponent == 1 ? SharedEditionPool->push<BlockType::One>() : SharedEditionPool->push<BlockType::Zero>();
    }
    EditionReference addition = SharedEditionPool->push<BlockType::Addition>(0);
    for (const Tree* child : expression->children()) {
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
  return SharedEditionPool->push<BlockType::Zero>();
}

std::pair<EditionReference, uint8_t> Polynomial::MonomialCoefficient(const Tree* expression, const Tree* variable) {
  EditionPool * SharedEditionPool = SharedEditionPool;
  if (Comparison::AreEqual(expression, variable)) {
    return std::make_pair(SharedEditionPool->push<BlockType::One>(), 1);
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Tree* base = expression.nextNode();
    Tree* exponent = base.nextTree();
    if (Comparison::AreEqual(exponent, variable) && Integer::IsUint8(exponent)) {
      assert(Integer::Uint8(exponent) > 1);
      return std::make_pair(SharedEditionPool->push<BlockType::One>(), Integer::Uint8(exponent));
    }
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<const Tree *, int> indexedNode : NodeIterator::Children<NoEditable>(expression)) {
      Tree* child = std::get<const Tree *>(indexedNode);
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        EditionReference multCopy = EditionReference::Clone(expression);
        multCopy.childAtIndex(std::get<int>(indexedNode)).moveTreeOverTree(childCoefficient);
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
}  // namespace PoincareJ
