#include "polynomial.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
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
  Tree* pol(SharedEditionPool->push<BlockType::Polynomial>(1));
  pol->cloneTreeAfterNode(variable);
  return pol;
}

Tree* Polynomial::PushMonomial(const Tree* variable, uint8_t exponent,
                               const Tree* coefficient) {
  if (exponent == 0) {
    return (1_e)->clone();
  }
  if (!coefficient) {
    // Writing 1_e directly in the declaration does not work with clang
    coefficient = 1_e;
  }
  Tree* pol = PushEmpty(variable);
  return AddMonomial(pol, std::make_pair(coefficient->clone(), exponent));
}

Tree* Polynomial::LeadingIntegerCoefficient(Tree* polynomial) {
  Tree* result = polynomial;
  while (result->isPolynomial()) {
    result = LeadingCoefficient(result);
  }
  return result;
}

uint8_t Polynomial::ExponentAtIndex(const Tree* polynomial, int index) {
  assert(index >= 0 && index < NumberOfTerms(polynomial));
  return polynomial->nodeValue(1 + index);
}

void Polynomial::SetExponentAtIndex(Tree* polynomial, int index,
                                    uint8_t exponent) {
  polynomial->setNodeValue(1 + index, exponent);
}

void Polynomial::InsertExponentAtIndex(Tree* polynomial, int index,
                                       uint8_t exponent) {
  ValueBlock* exponentsAddress = polynomial->nodeValueBlock(1 + index);
  SharedEditionPool->insertBlock(exponentsAddress, ValueBlock(exponent), true);
}

void Polynomial::RemoveExponentAtIndex(Tree* polynomial, int index) {
  ValueBlock* exponentsAddress = polynomial->nodeValueBlock(1 + index);
  SharedEditionPool->removeBlocks(exponentsAddress, 1);
}

Tree* Polynomial::AddMonomial(Tree* polynomial,
                              std::pair<Tree*, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient = std::get<Tree*>(monomial);
  EditionReference polynomialReference(polynomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  for (int i = 0; i <= nbOfTerms; i++) {
    int16_t exponentOfChildI =
        i < nbOfTerms ? ExponentAtIndex(polynomialReference, i) : -1;
    if (exponent < exponentOfChildI) {
      continue;
    } else if (exponent == exponentOfChildI) {
      EditionReference previousChild = polynomialReference->child(i);
      Tree* currentCoefficient = previousChild->nextTree();
      Tree* addition = Polynomial::Addition(currentCoefficient, coefficient);
      previousChild->nextTree()->moveTreeBeforeNode(addition);
    } else {
      NAry::AddChildAtIndex(polynomialReference, coefficient, i + 1);
      InsertExponentAtIndex(polynomialReference, i, exponent);
    }
    break;
  }
  return polynomialReference;
}

Tree* Polynomial::Addition(Tree* polA, Tree* polB) {
  return Operation(polA, polB, BlockType::Addition, AddMonomial,
                   [](Tree* result, Tree* polynomial,
                      std::pair<Tree*, uint8_t> monomial, bool isLastTerm) {
                     EditionReference resultRef(result);
                     polynomial = AddMonomial(polynomial, monomial);
                     return resultRef->moveTreeOverTree(polynomial);
                   });
}

Tree* Polynomial::Multiplication(Tree* polA, Tree* polB) {
  // TODO: implement Kronecker-Shönhage trick?
  return Operation(
      polA, polB, BlockType::Multiplication, MultiplicationMonomial,
      [](Tree* result, Tree* polynomial, std::pair<Tree*, uint8_t> monomial,
         bool isLastTerm) {
        EditionReference resultRef(result);
        Tree* polynomialClone = isLastTerm ? polynomial : polynomial->clone();
        Tree* multiplication =
            MultiplicationMonomial(polynomialClone, monomial);
        return Addition(resultRef, multiplication);
      });
}

Tree* Polynomial::Subtraction(Tree* polA, Tree* polB) {
  return Addition(polA, Multiplication(polB, (-1_e)->clone()));
}

Tree* Polynomial::Operation(Tree* polA, Tree* polB, BlockType blockType,
                            OperationMonomial operationMonomial,
                            OperationReduce operationMonomialAndReduce) {
  if (!polA->isPolynomial()) {
    if (!polB->isPolynomial()) {
      EditionReference op =
          blockType == BlockType::Addition
              ? SharedEditionPool->push<BlockType::Addition>(2)
              : SharedEditionPool->push<BlockType::Multiplication>(2);
      // We're about to move polynomes around, we need references
      EditionReference polARef(polA);
      op->moveTreeAfterNode(polB);
      op->moveTreeAfterNode(polARef);
      Simplification::DeepSystematicReduce(op);
      return op;
    }
    return Operation(polB, polA, blockType, operationMonomial,
                     operationMonomialAndReduce);
  }
  const Tree* x = Variable(polA);
  if (polB->isPolynomial() && Comparison::Compare(x, Variable(polB)) > 0) {
    return Operation(polB, polA, blockType, operationMonomial,
                     operationMonomialAndReduce);
  }
  if (!polB->isPolynomial() || !Comparison::AreEqual(x, Variable(polB))) {
    polA =
        operationMonomial(polA, std::make_pair(polB, static_cast<uint8_t>(0)));
  } else {
    // Both polA and polB are polynom(x)
    EditionReference a = polA;
    EditionReference b = polB;
    Tree* variableB = b->nextNode();
    EditionReference coefficientB = variableB->nextTree();
    size_t i = 0;
    uint8_t nbOfTermsB = NumberOfTerms(b);
    variableB->removeTree();
    EditionReference result((0_e)->clone());
    assert(i < nbOfTermsB);
    while (i < nbOfTermsB) {
      EditionReference nextCoefficientB = coefficientB->nextTree();
      result = operationMonomialAndReduce(
          result, a, std::make_pair(coefficientB, ExponentAtIndex(b, i)),
          i == nbOfTermsB - 1);
      coefficientB = nextCoefficientB;
      i++;
    }
    // polB children have been pilfered; remove the node and the variable child
    b->removeNode();
    // polA has been merged in result
    polA = result;
  }
  return Sanitize(polA);
}

Tree* Polynomial::MultiplicationMonomial(Tree* polynomial,
                                         std::pair<Tree*, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  EditionReference coefficient(std::get<Tree*>(monomial));
  int nbOfTerms = NumberOfTerms(polynomial);
  assert(0 < nbOfTerms);
  EditionReference polynomialReference(polynomial);
  for (int i = 0; i < nbOfTerms; i++) {
    // * x^exponent
    SetExponentAtIndex(polynomialReference, i,
                       ExponentAtIndex(polynomialReference, i) + exponent);
    // * coefficient
    EditionReference previousChild = polynomialReference->child(i);
    Tree* currentCoefficient = previousChild->nextTree();
    // Avoid one cloning for last term
    Tree* coeffClone = i == nbOfTerms - 1 ? static_cast<Tree*>(coefficient)
                                          : coefficient->clone();
    Tree* multiplication =
        Polynomial::Multiplication(currentCoefficient, coeffClone);
    previousChild->nextTree()->moveTreeBeforeNode(multiplication);
  }
  return polynomialReference;
}

static void extractDegreeAndLeadingCoefficient(Tree* pol, const Tree* x,
                                               uint8_t* degree,
                                               EditionReference* coefficient) {
  if (pol->isPolynomial() &&
      Comparison::AreEqual(x, Polynomial::Variable(pol))) {
    *degree = Polynomial::Degree(pol);
    *coefficient = Polynomial::LeadingCoefficient(pol);
  } else {
    *degree = 0;
    *coefficient = pol;
  }
}

DivisionResult<Tree*> Polynomial::PseudoDivision(Tree* polA, Tree* polB) {
  EditionReference a(polA);
  if (!polA->isPolynomial() && !polB->isPolynomial()) {
    assert(polA->isInteger() && polB->isInteger());
    DivisionResult<Tree*> divisionResult = IntegerHandler::Division(
        Integer::Handler(polA), Integer::Handler(polB));
    EditionReference quotient = divisionResult.quotient;
    EditionReference remainder = divisionResult.remainder;
    polB->removeTree();
    if (Number::IsZero(remainder)) {
      a->removeTree();
      return {quotient, remainder};
    }
    quotient->removeTree();
    remainder->removeTree();
    return {.quotient = (0_e)->clone(), .remainder = a};
  }
  if (!polA->isPolynomial()) {
    polB->removeTree();
    return {.quotient = (0_e)->clone(), .remainder = a};
  }
  const Tree* var = Variable(a);
  if (polB->isPolynomial() && Comparison::Compare(var, Variable(polB)) >= 0) {
    var = Variable(polB);
  }
  EditionReference b(polB);
  EditionReference x = var->clone();
  uint8_t degreeA, degreeB;
  EditionReference leadingCoeffA, leadingCoeffB;
  extractDegreeAndLeadingCoefficient(a, x, &degreeA, &leadingCoeffA);
  extractDegreeAndLeadingCoefficient(b, x, &degreeB, &leadingCoeffB);
  EditionReference currentQuotient(0_e);
  while (degreeA >= degreeB) {
    DivisionResult result =
        PseudoDivision(leadingCoeffA->clone(), leadingCoeffB->clone());
    EditionReference quotient = result.quotient;
    EditionReference remainder = result.remainder;
    bool stopCondition = !Number::IsZero(remainder);
    remainder->removeTree();
    if (stopCondition) {
      quotient->removeTree();
      break;
    }
    EditionReference xPowerDegAMinusDegB =
        Polynomial::PushMonomial(x->clone(), degreeA - degreeB);
    currentQuotient = Polynomial::Addition(
        currentQuotient, Polynomial::Multiplication(
                             quotient->clone(), xPowerDegAMinusDegB->clone()));
    a = Polynomial::Subtraction(
        a, Polynomial::Multiplication(
               quotient,
               Polynomial::Multiplication(b->clone(), xPowerDegAMinusDegB)));
    extractDegreeAndLeadingCoefficient(a, x, &degreeA, &leadingCoeffA);
  }
  b->removeTree();
  x->removeTree();
  return {.quotient = currentQuotient, .remainder = a};
}

void Polynomial::Inverse(Tree* pol) {
  if (!pol->isPolynomial()) {
    Integer::SetSign(pol, Integer::Sign(pol) == NonStrictSign::Positive
                              ? NonStrictSign::Negative
                              : NonStrictSign::Positive);
    return;
  }
  for (int i = 1; i <= NumberOfTerms(pol); i++) {
    Inverse(pol->child(i));
  }
}

void Polynomial::Normalize(Tree* pol) {
  Tree* leadingIntCoeff = LeadingIntegerCoefficient(pol);
  if (Integer::Sign(leadingIntCoeff) == NonStrictSign::Negative) {
    Inverse(pol);
  }
}

Tree* Polynomial::Sanitize(Tree* polynomial) {
  uint8_t nbOfTerms = NumberOfTerms(polynomial);
  size_t i = 0;
  EditionReference coefficient = polynomial->child(1);
  while (i < nbOfTerms) {
    EditionReference nextCoefficient = coefficient->nextTree();
    if (Number::IsZero(coefficient)) {
      coefficient->removeTree();
      NAry::SetNumberOfChildren(polynomial, polynomial->numberOfChildren() - 1);
      RemoveExponentAtIndex(polynomial, i);
    }
    coefficient = nextCoefficient;
    i++;
  }
  int numberOfTerms = NumberOfTerms(polynomial);
  if (numberOfTerms == 0) {
    return polynomial->cloneTreeOverTree(0_e);
  }
  if (numberOfTerms == 1 && ExponentAtIndex(polynomial, 0) == 0) {
    EditionReference result(polynomial->child(1));
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

bool PolynomialParser::ContainsVariable(const Tree* tree) {
  int numberOfChildren = tree->numberOfChildren();
  if (numberOfChildren == 0) {
    return tree->isOfType({BlockType::UserFunction, BlockType::UserSequence,
                           BlockType::UserSymbol, BlockType::Constant,
                           BlockType::Variable});
  }
  const Tree* child = tree->nextNode();
  for (int i = 0; i < numberOfChildren; i++) {
    if (ContainsVariable(child)) {
      return true;
    }
    child = child->nextTree();
  }
  return false;
}

void PolynomialParser::AddVariable(Tree* set, const Tree* variable) {
  if (ContainsVariable(variable)) {
    Set::Add(set, variable);
  }
}

Tree* PolynomialParser::GetVariables(const Tree* expression) {
  Tree* variables = SharedEditionPool->push<BlockType::Set>(0);
  if (expression->isInteger()) {  // TODO: generic belongToField?
    return variables;
  }
  BlockType type = expression->type();
  // TODO: match
  if (type == BlockType::Power) {
    const Tree* base = expression->nextNode();
    const Tree* exponent = base->nextTree();
    assert(exponent->isInteger());
    assert(!Integer::Is<uint8_t>(exponent) ||
           Integer::Handler(exponent).to<uint8_t>() > 1);
    AddVariable(variables, Integer::Is<uint8_t>(exponent) ? base : expression);
  } else if (type == BlockType::Addition || type == BlockType::Multiplication ||
             type == BlockType::Complex) {
    for (const Tree* child : expression->children()) {
      if (child->isAddition() && type != BlockType::Complex &&
          type != BlockType::Complex) {
        AddVariable(variables, child);
      } else {
        // TODO: variables isn't expected to actually change.
        variables = Set::Union(variables, GetVariables(child));
      }
    }
  } else {
    AddVariable(variables, expression);
  }
  return variables;
}

Tree* PolynomialParser::RecursivelyParse(Tree* expression,
                                         const Tree* variables,
                                         size_t variableIndex) {
  const Tree* variable = nullptr;
  for (std::pair<const Tree*, int> indexedVariable :
       NodeIterator::Children<NoEditable>(variables)) {
    if (std::get<int>(indexedVariable) < variableIndex) {
      // Skip previously handled variable
      continue;
    }
    variableIndex += 1;
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
  for (auto [child, index] : NodeIterator::Children<Editable>(expression)) {
    if (index == 0) {
      // Pass variable child
      continue;
    }
    RecursivelyParse(child, variables, variableIndex);
  }
  return expression;
}

Tree* PolynomialParser::Parse(Tree* expression, const Tree* variable) {
  EditionReference polynomial(Polynomial::PushEmpty(variable));
  BlockType type = expression->type();
  if (type == BlockType::Addition) {
    for (size_t i = 0; i < expression->numberOfChildren(); i++) {
      /* We deplete the addition from its children as we scan it so we can
       * always take the first child. */
      Tree* child = expression->nextNode();
      auto parsedChild = ParseMonomial(child, variable);
      polynomial = Polynomial::AddMonomial(polynomial, parsedChild);
    }
    polynomial = Polynomial::Sanitize(polynomial);
    // Addition node has been emptied from children
    expression->moveTreeOverNode(polynomial);
  } else {
    // Move polynomial next to expression before it's parsed (and likely
    // replaced)
    EditionReference expressionRef(expression);
    expressionRef->moveTreeBeforeNode(polynomial);
    polynomial = Polynomial::AddMonomial(
        polynomial, ParseMonomial(expressionRef, variable));
    polynomial = Polynomial::Sanitize(polynomial);
  }
  return polynomial;
}

std::pair<Tree*, uint8_t> PolynomialParser::ParseMonomial(
    Tree* expression, const Tree* variable) {
  if (Comparison::AreEqual(expression, variable)) {
    return std::make_pair(expression->cloneTreeOverTree(1_e),
                          static_cast<uint8_t>(1));
  }
  PatternMatching::Context ctx = PatternMatching::Context({.KA = variable});
  if (PatternMatching::Match(KPow(KA, KB), expression, &ctx)) {
    const Tree* exponent = ctx.getNode(KB);
    if (Integer::Is<uint8_t>(exponent)) {
      uint8_t exp = Integer::Handler(exponent).to<uint8_t>();
      assert(exp > 1);
      return std::make_pair(expression->cloneTreeOverTree(1_e), exp);
    }
  }
  if (expression->isMultiplication()) {
    for (auto [child, index] : NodeIterator::Children<Editable>(expression)) {
      auto [childCoefficient, childExponent] =
          ParseMonomial(child->clone(), variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at
        // basicReduction
        /* TODO: if the previous assertion is wrong, we have to multiply
         * children coefficients and addition children exponents. */
        child->moveTreeOverTree(childCoefficient);
        Simplification::DeepSystematicReduce(expression);
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
  return variables.isSet() ?
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
    if (Integer::Is<uint8_t>(exponent) && Compare::AreEqual(base, variable)) {
      return Integer::Handler(exponent).to<uint8_t>();
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
  if (expression.isAddition()) {
    if (Comparison::AreEqual(expression, variable)) {
      return exponent == 1 ? 1_e : 0_e;
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
  return 0_e;
}

std::pair<EditionReference, uint8_t> Polynomial::MonomialCoefficient(const Tree* expression, const Tree* variable) {
  if (Comparison::AreEqual(expression, variable)) {
    return std::make_pair((1_e)->clone(), 1);
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Tree* base = expression.nextNode();
    Tree* exponent = base.nextTree();
    if (Comparison::AreEqual(exponent, variable) && Integer::Is<uint8_t>(exponent)) {
      assert(Integer::Handler(exponent).to<uint8_t>() > 1);
      return std::make_pair((1_e)->clone(), Integer::Handler(exponent).to<uint8_t>());
    }
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<const Tree *, int> indexedNode : NodeIterator::Children<NoEditable>(expression)) {
      Tree* child = std::get<const Tree *>(indexedNode);
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        EditionReference multCopy = EditionReference::Clone(expression);
        multCopy.child(std::get<int>(indexedNode)).moveTreeOverTree(childCoefficient);
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
