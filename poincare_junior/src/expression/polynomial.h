#ifndef POINCARE_EXPRESSION_POLYNOMIAL_H
#define POINCARE_EXPRESSION_POLYNOMIAL_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "k_tree.h"

namespace PoincareJ {

class Polynomial final {
  /* We opt for the recursive representation.
   * - Node:
   *  | P TAG | number of terms | highest exponant | 2nd highest exponant | ...
   *
   * - Children: the first child is the variable, the others are the
   * coefficients corresponding at each exponant.
   *
   * TODO:
   * - Polynomial could have no representation and we would just implement
   *   functions to get the variables, exponents and coefficients dynamically.
   * - Polynomial could have a monomial (non-recursive) sparse representation.
   * Would it speed up polynomial algorithms (GCD, Gröbner basis)? Monomial
   * representation:
   * - Polynomial P = a0*x0^e0(x0)*x1^e0(x1)*... + a1*x0^e1(x0)*x1^e1(x1)*... +
   *   n = number of variables
   *   m = number of terms
   *   ei(xi) are uint8_t
   *   a0 are int32_t
   *  | P TAG | n | m | e0(x0) | e0(x1) | ... | e1(x0) | e1(x1) | ...
   *  This node has n children: the first n children describe the variables,
   *  the next m children describe the coefficients.
   */
  friend class PolynomialParser;

 public:
  static Tree* PushEmpty(const Tree* variable);
  static Tree* PushMonomial(const Tree* variable, uint8_t exponent,
                            const Tree* coefficient = nullptr);

  // Getters
  static uint8_t ExponentAtIndex(const Tree* polynomial, int index);
  static uint8_t Degree(const Tree* polynomial) {
    return ExponentAtIndex(polynomial, 0);
  }
  static Tree* LeadingCoefficient(Tree* polynomial) {
    assert(NumberOfTerms(polynomial) > 0);
    return polynomial->child(1);
  }
  static Tree* LeadingIntegerCoefficient(Tree* polynomial);
  static uint8_t NumberOfTerms(const Tree* polynomial) {
    assert(polynomial->isPolynomial());
    return polynomial->numberOfChildren() - 1;
  }
  static const Tree* Variable(const Tree* polynomial) {
    assert(polynomial->isPolynomial());
    return polynomial->child(0);
  }

  // Setters
  static void SetExponentAtIndex(Tree* polynomial, int index, uint8_t exponent);
  static void InsertExponentAtIndex(Tree* polynomial, int index,
                                    uint8_t exponent);
  static void RemoveExponentAtIndex(Tree* polynomial, int index);

  // Operations
  static Tree* AddMonomial(Tree* polynomial,
                           std::pair<Tree*, uint8_t> monomial);
  // Operations consume both polynomials
  static Tree* Addition(Tree* polA, Tree* polB);
  static Tree* Multiplication(Tree* polA, Tree* polB);
  static Tree* Subtraction(Tree* polA, Tree* polB);
  /* Pseudo-division
   * NB: the order of variables affects the result of the pseudo division.
   * A = Q*B + R with either deg(R) < deg(Q) or lc(R) is not a divisor of lc(B)
   * If the second condition stops the iteration, the representation is not
   * unique. If B is a divisor of A, the process terminates at the unique
   * representation A = B*Q.
   * Ex: x^2y^2+x = xy * (xy+1) + x-xy if we consider the variable x first
   * variable and  x^2y^2+x = (xy-1)*(xy+1) + x+1 if y is the first variable. */
  static DivisionResult<Tree*> PseudoDivision(Tree* polA, Tree* polB);

  // In-place transformations
  static void Inverse(Tree* pol);
  static void Normalize(Tree* pol);

 private:
  // Discard null term and potentially discard the polynomial structure
  static Tree* Sanitize(Tree* pol);
  typedef Tree* (*OperationMonomial)(Tree* polynomial,
                                     std::pair<Tree*, uint8_t> monomial);
  typedef Tree* (*OperationReduce)(Tree* result, Tree* polynomial,
                                   std::pair<Tree*, uint8_t> monomial,
                                   bool isLastTerm);
  static Tree* Operation(Tree* polA, Tree* polB, BlockType type,
                         OperationMonomial operationMonomial,
                         OperationReduce operationMonomialAndReduce);
  static Tree* MultiplicationMonomial(Tree* pol,
                                      std::pair<Tree*, uint8_t> monomial);
};

class PolynomialParser final {
 public:
  static bool ContainsVariable(const Tree* tree);
  static Tree* GetVariables(const Tree* expression);
  static Tree* RecursivelyParse(Tree* expression, const Tree* variables,
                                size_t variableIndex = 0);
  static Tree* Parse(Tree* expression, const Tree* variable);

 private:
  static void AddVariable(Tree* set, const Tree* variable);
  static std::pair<Tree*, uint8_t> ParseMonomial(Tree* expression,
                                                 const Tree* variable);
#if 0
  Tree* PolynomialInterpretation
  Tree* RationalInterpretation --> list of 2 polynomial
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
  Tree* m_listOfVariables; // Set of expressions
  Tree* ListOfCoefficient; // Integers -> based of set of expressions order
  Tree* ListOfListsExponents; // List of list of exponents
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
