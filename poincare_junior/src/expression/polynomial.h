#ifndef POINCARE_EXPRESSION_POLYNOMIAL_H
#define POINCARE_EXPRESSION_POLYNOMIAL_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Polynomial final {
  friend class PolynomialParser;

 public:
  static EditionReference PushEmpty(EditionReference variable);
  static EditionReference PushMonomial(
      EditionReference variable, uint8_t exponent,
      EditionReference coefficient = EditionReference(&OneBlock));

  // Getters
  static uint8_t ExponentAtIndex(const Node polynomial, int index);
  static uint8_t Degree(const Node polynomial) {
    return ExponentAtIndex(polynomial, 0);
  }
  static EditionReference LeadingCoefficient(EditionReference polynomial) {
    assert(NumberOfTerms(polynomial) > 0);
    return polynomial.childAtIndex(1);
  }
  static uint8_t NumberOfTerms(const Node polynomial) {
    assert(polynomial.type() == BlockType::Polynomial);
    return polynomial.numberOfChildren() - 1;
  }
  static EditionReference Variable(const Node polynomial) {
    assert(polynomial.type() == BlockType::Polynomial);
    return polynomial.childAtIndex(0);
  }

  // Setters
  static void SetExponentAtIndex(EditionReference polynomial, int index,
                                 uint8_t exponent);
  static void InsertExponentAtIndex(EditionReference polynomial, int index,
                                    uint8_t exponent);
  static void RemoveExponentAtIndex(EditionReference polynomial, int index);

  // Operations
  // *x^n
  // * lambda
  // monomial c*x^n
  // si variable != --> polynom(biggest variable) * lambda
  // si variable == --> plusieurs monomial
  static void AddMonomial(EditionReference polynomial,
                          std::pair<EditionReference, uint8_t> monomial);
  // Operations consume both polynomials
  static EditionReference Addition(EditionReference polA,
                                   EditionReference polB);
  static EditionReference Multiplication(EditionReference polA,
                                         EditionReference polB);
  static EditionReference Subtraction(EditionReference polA,
                                      EditionReference polB);
  //
  // monomial multiplication
  // Computation
  // Unit normal GCD of coefficients
  // static EditionReference Content(EditionReference polynomial);
  /* Pseudo-division
   * NB: the order of variables affects the result of the pseudo division.
   * A = Q*B + R with either deg(R) < deg(Q) or lc(R) is not a divisor of lc(B)
   * If the second condition stops the iteration, the representation is not
   * unique. If B is a divisor of A, the process terminates at the unique
   * representation A = B*Q.
   * Ex: x^2y^2+x = xy * (xy+1) + x-xy if we consider the variable x first
   * variable and  x^2y^2+x = (xy-1)*(xy+1) + x+1 if y is the first variable. */
  static std::pair<EditionReference, EditionReference> PseudoDivision(
      EditionReference polA, EditionReference polB);
  // GCD
  // static Edi
 private:
  // Discard null term and potentially discard the polynomial structure
  static EditionReference Sanitize(EditionReference pol);
  typedef void (*OperationMonomial)(
      EditionReference polynomial,
      std::pair<EditionReference, uint8_t> monomial);
  typedef EditionReference (*OperationReduce)(
      EditionReference result, EditionReference polynomial,
      std::pair<EditionReference, uint8_t> monomial, bool isLastTerm);
  static EditionReference Operation(EditionReference polA,
                                    EditionReference polB, BlockType type,
                                    OperationMonomial operationMonomial,
                                    OperationReduce operationMonomialAndReduce);
  static void MultiplicationMonomial(
      EditionReference pol, std::pair<EditionReference, uint8_t> monomial);
};

class PolynomialParser final {
  /* TODO: Polynomial could have their own sparse representation to speed up
   * polynomial GCD, Grobner basis... But this would require to implement their
   * own operations.
   */

 public:
  static EditionReference GetVariables(const Node expression);
  static EditionReference RecursivelyParse(EditionReference expression,
                                           EditionReference variables,
                                           size_t variableIndex = 0);
  // static uint8_t Degree(const Node expression, const Node variable);
  // static EditionReference Coefficient(const Node expression, const Node
  // variable, uint8_t exponent); static  LeadingCoefficient(const Node
  // expression, const Node variable);
  /* Parsing polynomial:
   * - getVariables
   * - n0 = degree in x0
   * - a = coefficient in x0^n
   * - n1 = degree in x1 of a
   * - a = coefficent in x1^n1
   *   when no variable anymore, a = coefficient of x0^n*x1^m
   *
   *   DECIDE monomial / recursive*/
 private:
  static EditionReference Parse(EditionReference expression,
                                EditionReference variable);
  static std::pair<EditionReference, uint8_t> ParseMonomial(
      EditionReference expression, EditionReference variable);
#if 0
  Node PolynomialInterpretation
  Node RationalInterpretation --> list of 2 polynomial
  // Set!
  //
  sign
  compare

  // General polynomial
  isMonomial(u,setOfGeneralVariables)
  isPolynomial(u,setOfGeneralVariables)
  coefficientMonomial(u, generalVariable)
  coefficient(u, generalVariable, exponent (int))
  leadingCoefficient(u,generalVariable)
  collectTerms(u, S) //--> polynomial form in S
  ALGEBRAIC_EXPAND // Should we apply on all subexpressions? --> NO agit uniquement sur +*^

  Numerator
  Denominator
private:
  exponentForNthVariable(int monomialIndex, int variableIndex)
  Integer coeffient(int monomialIndex);
  Node m_listOfVariables; // Set of expressions
  Node ListOfCoefficient; // Integers -> based of set of expressions order
  Node ListOfListsExponents; // List of list of exponents
#endif
};

}  // namespace PoincareJ

#endif
/*
algebraic_reduction
--> rationalize
--> numerator Pe = expr
--> denominator Qe = expr
for numerator and denominator:
--> algebraic_expand Pe
--> S = variables(Pe)
--> extract side relation for S
--> Pp = polynome(Pe) (collectTerms ?)
--> polynomial expansion in Grobner base? for Pp

--> S2 = union of variables(P) U variables(Q)
--> Pp2 = polynome(Pe), Qp2 = polynome(Qe)
--> polynome division Pp2/Qp2 per GCD


resultant

leading_monomial(u, L)
Mb_poly(div, u, V, L)*/
