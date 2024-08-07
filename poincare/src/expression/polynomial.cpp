#include "polynomial.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/value_block.h>

#include "advanced_reduction.h"
#include "approximation.h"
#include "dependency.h"
#include "k_tree.h"
#include "number.h"
#include "rational.h"
#include "set.h"
#include "sign.h"
#include "simplification.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

/* Polynomial */

Tree* Polynomial::PushEmpty(const Tree* variable) {
  Tree* pol = SharedTreeStack->pushBlock(Type::Polynomial);
  SharedTreeStack->pushBlock(1);
  pol->cloneTreeAfterNode(variable);
  return pol;
}

Tree* Polynomial::PushMonomial(const Tree* variable, uint8_t exponent,
                               const Tree* coefficient) {
  if (exponent == 0) {
    return (1_e)->cloneTree();
  }
  if (!coefficient) {
    // Writing 1_e directly in the declaration does not work with clang
    coefficient = 1_e;
  }
  Tree* pol = PushEmpty(variable);
  return AddMonomial(pol, std::make_pair(coefficient->cloneTree(), exponent));
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
  SharedTreeStack->insertBlock(exponentsAddress, ValueBlock(exponent), true);
}

void Polynomial::RemoveExponentAtIndex(Tree* polynomial, int index) {
  ValueBlock* exponentsAddress = polynomial->nodeValueBlock(1 + index);
  SharedTreeStack->removeBlocks(exponentsAddress, 1);
}

Tree* Polynomial::AddMonomial(Tree* polynomial,
                              std::pair<Tree*, uint8_t> monomial) {
  uint8_t exponent = std::get<uint8_t>(monomial);
  TreeRef coefficient = std::get<Tree*>(monomial);
  TreeRef polynomialReference(polynomial);
  int nbOfTerms = NumberOfTerms(polynomial);
  for (int i = 0; i <= nbOfTerms; i++) {
    int16_t exponentOfChildI =
        i < nbOfTerms ? ExponentAtIndex(polynomialReference, i) : -1;
    if (exponent < exponentOfChildI) {
      continue;
    } else if (exponent == exponentOfChildI) {
      TreeRef previousChild = polynomialReference->child(i);
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
  return Operation(polA, polB, Type::Add, AddMonomial,
                   [](Tree* result, Tree* polynomial,
                      std::pair<Tree*, uint8_t> monomial, bool isLastTerm) {
                     TreeRef resultRef(result);
                     polynomial = AddMonomial(polynomial, monomial);
                     return resultRef->moveTreeOverTree(polynomial);
                   });
}

Tree* Polynomial::Multiplication(Tree* polA, Tree* polB) {
  // TODO: implement Kronecker-Shönhage trick?
  return Operation(polA, polB, Type::Mult, MultiplicationMonomial,
                   [](Tree* result, Tree* polynomial,
                      std::pair<Tree*, uint8_t> monomial, bool isLastTerm) {
                     TreeRef resultRef(result);
                     Tree* polynomialClone =
                         isLastTerm ? polynomial : polynomial->cloneTree();
                     Tree* multiplication =
                         MultiplicationMonomial(polynomialClone, monomial);
                     return Addition(resultRef, multiplication);
                   });
}

Tree* Polynomial::Subtraction(Tree* polA, Tree* polB) {
  return Addition(polA, Multiplication(polB, (-1_e)->cloneTree()));
}

Tree* Polynomial::Operation(Tree* polA, Tree* polB, Type type,
                            OperationMonomial operationMonomial,
                            OperationReduce operationMonomialAndReduce) {
  if (!polA->isPolynomial()) {
    if (!polB->isPolynomial()) {
      TreeRef op = type == Type::Add ? SharedTreeStack->pushAdd(2)
                                     : SharedTreeStack->pushMult(2);
      // We're about to move polynomes around, we need references
      TreeRef polARef(polA);
      op->moveTreeAfterNode(polB);
      op->moveTreeAfterNode(polARef);
      SystematicReduction::DeepReduce(op);
      return op;
    }
    return Operation(polB, polA, type, operationMonomial,
                     operationMonomialAndReduce);
  }
  const Tree* x = Variable(polA);
  if (polB->isPolynomial() && Order::Compare(x, Variable(polB)) > 0) {
    return Operation(polB, polA, type, operationMonomial,
                     operationMonomialAndReduce);
  }
  if (!polB->isPolynomial() || !x->treeIsIdenticalTo(Variable(polB))) {
    polA =
        operationMonomial(polA, std::make_pair(polB, static_cast<uint8_t>(0)));
  } else {
    // Both polA and polB are polynom(x)
    TreeRef a = polA;
    TreeRef b = polB;
    Tree* variableB = b->child(0);
    TreeRef coefficientB = variableB->nextTree();
    int i = 0;
    uint8_t nbOfTermsB = NumberOfTerms(b);
    variableB->removeTree();
    TreeRef result((0_e)->cloneTree());
    assert(i < nbOfTermsB);
    while (i < nbOfTermsB) {
      TreeRef nextCoefficientB = coefficientB->nextTree();
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
  TreeRef coefficient(std::get<Tree*>(monomial));
  int nbOfTerms = NumberOfTerms(polynomial);
  assert(0 < nbOfTerms);
  TreeRef polynomialReference(polynomial);
  for (int i = 0; i < nbOfTerms; i++) {
    // * x^exponent
    SetExponentAtIndex(polynomialReference, i,
                       ExponentAtIndex(polynomialReference, i) + exponent);
    // * coefficient
    TreeRef previousChild = polynomialReference->child(i);
    Tree* currentCoefficient = previousChild->nextTree();
    // Avoid one cloning for last term
    Tree* coeffClone = i == nbOfTerms - 1 ? static_cast<Tree*>(coefficient)
                                          : coefficient->cloneTree();
    Tree* multiplication =
        Polynomial::Multiplication(currentCoefficient, coeffClone);
    previousChild->nextTree()->moveTreeBeforeNode(multiplication);
  }
  return polynomialReference;
}

static void extractDegreeAndLeadingCoefficient(Tree* pol, const Tree* x,
                                               uint8_t* degree,
                                               TreeRef* coefficient) {
  if (pol->isPolynomial() && x->treeIsIdenticalTo(Polynomial::Variable(pol))) {
    *degree = Polynomial::Degree(pol);
    *coefficient = Polynomial::LeadingCoefficient(pol);
  } else {
    *degree = 0;
    *coefficient = pol;
  }
}

DivisionResult<Tree*> Polynomial::PseudoDivision(Tree* polA, Tree* polB) {
  TreeRef a(polA);
  if (!polA->isPolynomial() && !polB->isPolynomial()) {
    assert(polA->isInteger() && polB->isInteger());
    DivisionResult<Tree*> divisionResult = IntegerHandler::Division(
        Integer::Handler(polA), Integer::Handler(polB));
    TreeRef quotient = divisionResult.quotient;
    TreeRef remainder = divisionResult.remainder;
    polB->removeTree();
    if (remainder->isZero()) {
      a->removeTree();
      return {quotient, remainder};
    }
    quotient->removeTree();
    remainder->removeTree();
    return {.quotient = (0_e)->cloneTree(), .remainder = a};
  }
  if (!polA->isPolynomial()) {
    polB->removeTree();
    return {.quotient = (0_e)->cloneTree(), .remainder = a};
  }
  const Tree* var = Variable(a);
  if (polB->isPolynomial() && Order::Compare(var, Variable(polB)) >= 0) {
    var = Variable(polB);
  }
  TreeRef b(polB);
  TreeRef x = var->cloneTree();
  uint8_t degreeA, degreeB;
  TreeRef leadingCoeffA, leadingCoeffB;
  extractDegreeAndLeadingCoefficient(a, x, &degreeA, &leadingCoeffA);
  extractDegreeAndLeadingCoefficient(b, x, &degreeB, &leadingCoeffB);
  TreeRef currentQuotient(0_e);
  while (degreeA >= degreeB) {
    DivisionResult result =
        PseudoDivision(leadingCoeffA->cloneTree(), leadingCoeffB->cloneTree());
    TreeRef quotient = result.quotient;
    TreeRef remainder = result.remainder;
    bool stopCondition = !remainder->isZero();
    remainder->removeTree();
    if (stopCondition) {
      quotient->removeTree();
      break;
    }
    TreeRef xPowerDegAMinusDegB =
        Polynomial::PushMonomial(x->cloneTree(), degreeA - degreeB);
    currentQuotient = Polynomial::Addition(
        currentQuotient,
        Polynomial::Multiplication(quotient->cloneTree(),
                                   xPowerDegAMinusDegB->cloneTree()));
    a = Polynomial::Subtraction(
        a, Polynomial::Multiplication(
               quotient, Polynomial::Multiplication(b->cloneTree(),
                                                    xPowerDegAMinusDegB)));
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
  TreeRef coefficient = polynomial->child(1);
  while (i < nbOfTerms) {
    TreeRef nextCoefficient = coefficient->nextTree();
    if (coefficient->isZero()) {
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
    TreeRef result(polynomial->child(1));
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

bool PolynomialParser::ContainsVariable(const Tree* e) {
  int numberOfChildren = e->numberOfChildren();
  if (numberOfChildren == 0) {
    return e->isOfType({Type::UserFunction, Type::UserSequence,
                        Type::UserSymbol, Type::Pi, Type::EulerE, Type::Var});
  }
  const Tree* child = e->child(0);
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

Tree* PolynomialParser::GetVariables(const Tree* e) {
  Tree* variables = SharedTreeStack->pushSet(0);
  if (e->isInteger()) {  // TODO: generic belongToField?
    return variables;
  }
  Type type = e->type();
  // TODO: match
  if (type == Type::Pow) {
    const Tree* base = e->child(0);
    const Tree* exponent = base->nextTree();
    assert(exponent->isInteger());
    assert(!Integer::Is<uint8_t>(exponent) ||
           Integer::Handler(exponent).to<uint8_t>() > 1);
    AddVariable(variables, Integer::Is<uint8_t>(exponent) ? base : e);
  } else if (type == Type::Add || type == Type::Mult) {
    for (const Tree* child : e->children()) {
      if (child->isAdd() && type != Type::Add) {
        AddVariable(variables, child);
      } else {
        // TODO: variables isn't expected to actually change.
        variables = Set::Union(variables, GetVariables(child));
      }
    }
  } else {
    AddVariable(variables, e);
  }
  return variables;
}

Tree* PolynomialParser::RecursivelyParse(Tree* e, const Tree* variables,
                                         size_t variableIndex) {
  const Tree* variable = nullptr;
  for (IndexedChild<const Tree*> indexedVariable :
       variables->indexedChildren()) {
    if (indexedVariable.index < static_cast<int>(variableIndex)) {
      // Skip previously handled variable
      continue;
    }
    variableIndex += 1;
    if (Order::ContainsSubtree(e, indexedVariable)) {
      variable = indexedVariable;
      break;
    }
  }
  if (!variable) {
    // e is not a polynomial of variables
    return e;
  }
  e = Parse(e, variable);
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (child.index == 0) {
      // Pass variable child
      continue;
    }
    RecursivelyParse(child, variables, variableIndex);
  }
  return e;
}

Tree* PolynomialParser::Parse(Tree* e, const Tree* variable) {
  assert(!AdvancedReduction::DeepExpand(e));
  TreeRef polynomial(Polynomial::PushEmpty(variable));
  Type type = e->type();
  if (type == Type::Add) {
    for (int i = 0; i < e->numberOfChildren(); i++) {
      /* We deplete the addition from its children as we scan it so we can
       * always take the first child. */
      Tree* child = e->child(0);
      auto parsedChild = ParseMonomial(child, variable);
      polynomial = Polynomial::AddMonomial(polynomial, parsedChild);
    }
    polynomial = Polynomial::Sanitize(polynomial);
    // Addition node has been emptied from children
    e->moveTreeOverNode(polynomial);
  } else {
    // Move polynomial next to e before it's parsed (and likely
    // replaced)
    TreeRef eRef(e);
    eRef->moveTreeBeforeNode(polynomial);
    polynomial =
        Polynomial::AddMonomial(polynomial, ParseMonomial(eRef, variable));
    polynomial = Polynomial::Sanitize(polynomial);
  }
  return polynomial;
}

std::pair<Tree*, uint8_t> PolynomialParser::ParseMonomial(
    Tree* e, const Tree* variable) {
  if (e->treeIsIdenticalTo(variable)) {
    return std::make_pair(e->cloneTreeOverTree(1_e), static_cast<uint8_t>(1));
  }
  PatternMatching::Context ctx = PatternMatching::Context({.KA = variable});
  if (PatternMatching::Match(e, KPow(KA, KB), &ctx)) {
    const Tree* exponent = ctx.getTree(KB);
    if (Integer::Is<uint8_t>(exponent)) {
      uint8_t exp = Integer::Handler(exponent).to<uint8_t>();
      assert(exp > 1);
      return std::make_pair(e->cloneTreeOverTree(1_e), exp);
    }
  }
  if (e->isMult()) {
    for (Tree* child : e->children()) {
      auto [childCoefficient, childExponent] =
          ParseMonomial(child->cloneTree(), variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at
        // basicReduction
        /* TODO: if the previous assertion is wrong, we have to multiply
         * children coefficients and addition children exponents. */
        child->moveTreeOverTree(childCoefficient);
        SystematicReduction::DeepReduce(e);
        return std::make_pair(e, childExponent);
      }
      childCoefficient->removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(!Order::ContainsSubtree(e, variable));
  return std::make_pair(e, static_cast<uint8_t>(0));
}

Tree* PolynomialParser::GetCoefficients(const Tree* e, const char* symbolName) {
  Tree* symbol = SharedTreeStack->pushUserSymbol(symbolName);
  Tree* poly = e->cloneTree();
  // TODO: add dependency on each coef?
  Dependency::RemoveDependencies(poly);
  AdvancedReduction::DeepExpand(poly);
  Dependency::RemoveDependencies(poly);
  poly = Parse(poly, symbol);
  if (!poly->isPolynomial()) {
    TreeRef result = SharedTreeStack->pushList(0);
    NAry::AddChild(result, poly);
    symbol->removeTree();
    return result;
  }
  int degree = Polynomial::Degree(poly);
  if (degree < 0 || degree > Polynomial::k_maxPolynomialDegree) {
    poly->removeTree();
    symbol->removeTree();
    return nullptr;
  }
  TreeRef result = SharedTreeStack->pushList(0);
  int numberOfTerms = Polynomial::NumberOfTerms(poly);
  int indexExponent = numberOfTerms - 1;
  for (int i = 0; i <= degree; i++) {
    if (0 <= indexExponent &&
        i == Polynomial::ExponentAtIndex(poly, indexExponent)) {
      NAry::AddChild(result, poly->child(indexExponent + 1)->cloneTree());
      indexExponent--;
    } else {
      NAry::AddChild(result, SharedTreeStack->pushZero());
    }
  }
  poly->removeTree();
  symbol->removeTree();
  return result;
}

Tree* PolynomialParser::GetReducedCoefficients(const Tree* e,
                                               const char* symbolName,
                                               bool keepDependencies) {
  Tree* coefList = PolynomialParser::GetCoefficients(e, symbolName);
  if (!coefList) {
    return nullptr;
  }
  for (Tree* child : coefList->children()) {
    Simplification::ReduceSystem(child, false);
    if (!keepDependencies && child->isDep()) {
      child->moveTreeOverTree(child->child(0));
    }
  }
  return coefList;
}

bool PolynomialParser::HasNonNullCoefficients(
    const Tree* e, const char* symbol,
    OMG::Troolean* highestDegreeCoefficientIsPositive) {
  Tree* coefList = GetReducedCoefficients(e, symbol, true);
  if (!coefList) {
    return false;
  }
  int degree = coefList->numberOfChildren() - 1;
  assert(0 <= degree && degree <= Polynomial::k_maxPolynomialDegree);

  if (highestDegreeCoefficientIsPositive) {
    const Tree* child = coefList->child(degree);
    ComplexSign sign = ComplexSign::Get(child);
    /* We should assert sign.isReal() but the sign is not always precise enough,
     * so check approximation is real. */
    assert(Dimension::Get(child).isScalar() &&
           (Approximation::ToComplex<double>(child).imag() == 0 ||
            std::isnan(Approximation::ToComplex<double>(child).imag())));
    assert(sign.realSign().trooleanIsNull() != OMG::Troolean::True);
    OMG::Troolean isPositive = sign.realSign().trooleanIsStrictlyPositive();
    if (isPositive == OMG::Troolean::Unknown) {
      // Approximate for a better estimation. Nan if coefficient depends on x/y.
      double approximation = Approximation::To<double>(child);
      if (!std::isnan(approximation)) {
        isPositive = OMG::BoolToTroolean(approximation > 0.0);
      }
    }
    *highestDegreeCoefficientIsPositive = isPositive;
  }

  for (const Tree* child : coefList->children()) {
    ComplexSign sign = ComplexSign::Get(child);
    /* We should assert sign.isReal() but the sign is not always precise enough,
     * so check approximation is real. */
    assert(Dimension::Get(child).isScalar() &&
           (Approximation::ToComplex<double>(child).imag() == 0 ||
            std::isnan(Approximation::ToComplex<double>(child).imag())));
    OMG::Troolean isNull = sign.realSign().trooleanIsNull();
    if (isNull == OMG::Troolean::Unknown) {
      // Same comment as above
      double approximation = Approximation::To<double>(child);
      if (!std::isnan(approximation)) {
        isNull = OMG::BoolToTroolean(approximation != 0.0);
      }
    }
    if (isNull == OMG::Troolean::False) {
      coefList->removeTree();
      return true;
    }
  }
  coefList->removeTree();
  return false;
}

#if 0
bool IsInSetOrIsEqual(const Tree* e, const Tree* variables) {
  return variables.isSet() ?
    Set::Includes(variables, e) :
    variables->treeIsIdenticalTo(e);
}

uint8_t Polynomial::Degree(const Tree* e, const Tree* variable) {
  if (e->treeIsIdenticalTo(variable)) {
    return 1;
  }
  Type type = e.type();
  if (type == Type::Pow) {
    Tree* base = e.child(0);
    Tree* exponent = base.nextTree();
    if (Integer::Is<uint8_t>(exponent) && base->treeIsIdenticalTo(variable)) {
      return Integer::Handler(exponent).to<uint8_t>();
    }
  }
  uint8_t degree = 0;
  if (type == Type::Add || type == Type::Mult) {
    for (const Tree* child : e->children()) {
      uint8_t childDegree = Degree(child, variables);
      if (type == Type::Add) {
        degree = std::max(degree, childDegree);
      } else {
        degree += childDegree;
      }
    }
  }
  assert(!e->isZero());
  return degree;
}

TreeRef Polynomial::Coefficient(const Tree* e, const Tree* variable, uint8_t exponent) {
  Type type = e.type();
  if (e.isAdd()) {
    if (e->treeIsIdenticalTo( variable)) {
      return exponent == 1 ? 1_e : 0_e;
    }
    TreeRef addition = SharedTreeStack->pushAdd(0);
    for (const Tree* child : e->children()) {
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent == exponent) {
        NAry::AddChild(addition, childCoefficient);
      } else {
        childCoefficient.removeTree();
      }
    }
    return addition; // TODO: apply basicReduction
  }
  auto [exprCoefficient, exprExponent] = MonomialCoefficient(e, variable);
  if (exponent == exprExponent) {
    return exprCoefficient;
  }
  exprCoefficient.removeTree();
  return 0_e;
}

std::pair<TreeRef, uint8_t> Polynomial::MonomialCoefficient(const Tree* e, const Tree* variable) {
  if (e->treeIsIdenticalTo( variable)) {
    return std::make_pair((1_e)->cloneTree(), 1);
  }
  Type type = e.type();
  if (type == Type::Pow) {
    Tree* base = e.child(0);
    Tree* exponent = base.nextTree();
    if (exponent->treeIsIdenticalTo( variable) && Integer::Is<uint8_t>(exponent)) {
      assert(Integer::Handler(exponent).to<uint8_t>() > 1);
      return std::make_pair((1_e)->cloneTree(), Integer::Handler(exponent).to<uint8_t>());
    }
  }
  if (type == Type::Mult) {
  for (IndexedChild<const Tree*> child :
       e->indexedChildren()) {
      auto [childCoefficient, childExponent] = MonomialCoefficient(child, variable);
      if (childExponent > 0) {
        // Warning: this algorithm relies on x^m*x^n --> x^(n+m) at basicReduction
        TreeRef multCopy = TreeRef::Clone(e);
        multCopy.child(child.index).moveTreeOverTree(childCoefficient);
        return std::make_pair(multCopy, childExponent);
      }
      childCoefficient.removeTree();
    }
  }
  // Assertion results from IsPolynomial = true
  assert(Order::ContainsSubtree(e, variable));
  return std::make_pair(TreeRef::Clone(e), 0);
}

#endif
}  // namespace Poincare::Internal
