#include <assert.h>
#include <poincare/helpers/layout.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/list.h>
#include <poincare/old/matrix.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/opposite.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>
#include <poincare/old/unit.h>

#include <utility>

namespace Poincare {

OMG::Troolean AdditionNode::isPositive(Context* context) const {
  if (numberOfChildren() < 1) {
    return OMG::Troolean::Unknown;
  }
  // If all children have same sign, addition has this sign too.
  OMG::Troolean additionIsPositive = childAtIndex(0)->isPositive(context);
  int childrenNumber = numberOfChildren();
  for (int i = 1; i < childrenNumber; i++) {
    if (childAtIndex(i)->isPositive(context) != additionIsPositive) {
      return OMG::Troolean::Unknown;
    }
  }
  return additionIsPositive;
}

int AdditionNode::getPolynomialCoefficients(Context* context,
                                            const char* symbolName,
                                            OExpression coefficients[]) const {
  return Addition(this).getPolynomialCoefficients(context, symbolName,
                                                  coefficients);
}

// Layout

size_t AdditionNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, "+");
}

// Simplication

OExpression AdditionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Addition(this).shallowReduce(reductionContext);
}

OExpression AdditionNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return Addition(this).shallowBeautify(reductionContext);
}

// Derivation
bool AdditionNode::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, OExpression symbolValue) {
  return Addition(this).derivate(reductionContext, symbol, symbolValue);
}

// Addition

const Number Addition::NumeralFactor(const OExpression& e) {
  if (e.otype() == ExpressionNode::Type::Multiplication &&
      e.childAtIndex(0).isNumber()) {
    Number result = e.childAtIndex(0).convert<Number>();
    return result;
  }
  return Rational::Builder(1);
}

int Addition::getPolynomialCoefficients(Context* context,
                                        const char* symbolName,
                                        OExpression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg <= 0 || deg > OExpression::k_maxPolynomialDegree) {
    return defaultGetPolynomialCoefficients(deg, context, symbolName,
                                            coefficients);
  }
  for (int k = 0; k < deg + 1; k++) {
    coefficients[k] = Addition::Builder();
  }
  OExpression intermediateCoefficients
      [OExpression::k_maxNumberOfPolynomialCoefficients];
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    int d = childAtIndex(i).getPolynomialCoefficients(context, symbolName,
                                                      intermediateCoefficients);
    assert(d <= deg);
    if (d < 0) {
      // We couldn't calculate child's polynomial coefficients
      return -1;
    }
    for (int j = 0; j < d + 1; j++) {
      static_cast<Addition&>(coefficients[j])
          .addChildAtIndexInPlace(intermediateCoefficients[j],
                                  coefficients[j].numberOfChildren(),
                                  coefficients[j].numberOfChildren());
    }
  }
  // No coefficient should be left as an empty addition.
  assert(coefficients[deg].numberOfChildren() > 0);
  return deg;
}

OExpression Addition::shallowBeautify(
    const ReductionContext& reductionContext) {
  /* Step 1 : Sort children in decreasing order of degree.
   * 1+x+x^3+y^2+x*y+sqrt(x) --> x^3+y^2+x*y+x+1+sqrt(x)
   * sqrt(2)+1 = 1+sqrt(2)
   *
   * First we consider the degree of symbols so that deg(x^2) > deg(x) > deg(1)
   * and deg(x*y) = deg(x^2). An expression without symbols has a degree of 0.
   * An expression which has an exponent that is not a number has a degree
   * of NAN (like x^y). It is put at the beginning of the addition.
   *
   * Second, if two terms of the addition have the same symbol degree,
   * we consider the exponent of their right-most term.
   * So deg(3*sqrt(2)) = 0.5, deg(1) = 0, deg(pi^3) = 3
   *
   * Third if two terms still have the same degre, we compare their bases.
   *
   * WARNING : This algorithm is not the cleanest possible but is a good
   * compromise between efficiency and exhaustivity.
   * It covers the main cases which are numbers (such as 2+3*sqrt(2))
   * and polynomials like (x + y + 1)^3 which will be sorted as :
   * x^3 + y^3 + 3*x*y^2 + 3*x^2*y + 3*x^2 + 3*y^2 + 6*x*y + 3*x + 3*y + 1
   *
   * What it does not handle, is expressions like x^sqrt(2) or x^pi,
   * since it does not approximate exponents that are not Numbers in Poincare.
   * These terms will just be put at the beginning of the addition.
   *
   * It also supposes that all terms are developped, so if we want to keep
   * factorized terms after the reduction, this won't work for sorting
   * additions like (x+1)^5 + x^2 + 1 (the degree of (x+1)^5 is not computed
   * yet in its factorized form, but it could be easily implemented if needed,
   * by implementing "degreeForSortingAddition" on additionNode.).
   *
   * The algorithm also does not sort 5*x+sqrt(2)*x as we might expect.
   * Since the two terms have the same symbol degree, they are sorted by
   * the degree of their right-most term, which is also equal.
   * So in the end it uses the simplificationOrder for sorting, which puts
   * sqrt(2)*x before 5*x. So 5*x+sqrt(2)*x = sqrt(2)*x+5*x
   * */

  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        /* Repeat twice, once for symbol degree, once for any degree */
        for (bool sortBySymbolDegree : {true, false}) {
          double e1Degree = e1->degreeForSortingAddition(sortBySymbolDegree);
          double e2Degree = e2->degreeForSortingAddition(sortBySymbolDegree);
          if (!std::isnan(e2Degree) &&
              (std::isnan(e1Degree) || e1Degree > e2Degree)) {
            return -1;
          }
          if (!std::isnan(e1Degree) &&
              (std::isnan(e2Degree) || e2Degree > e1Degree)) {
            return 1;
          }
        }
        // If they have same degree, sort children in decreasing order of base.
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      reductionContext.context(), reductionContext.shouldCheckMatrices());

  /* Step 2 : Add Subtractions if needed
   * We want to turn "a+(-1)*b" into "a-b". Or, more precisely, any
   * "a+(-r)*b" into "a-r*b" where r is a positive Rational.
   * Note: the process will slightly differ if the negative product occurs on
   * the first term: we want to turn "Addition(Multiplication(-1,b))" into
   * "Opposite(b)".
   * Special care must be taken when iterating over children since we may
   * remove some during the process. */
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    // Try to make the child i positive if any negative numeral factor is found
    OExpression subtractant =
        childAtIndex(i).makePositiveAnyNegativeNumeralFactor(reductionContext);
    if (subtractant.isUninitialized()) {
      /* if subtractant is not initialized, it means the child i had no negative
       * numeral factor we ignore terms which are not like "(-r)*a" */
      continue;
    }

    if (i == 0) {
      Opposite o = Opposite::Builder(subtractant);
      replaceChildAtIndexInPlace(i, o);
    } else {
      OExpression leftSibling = childAtIndex(i - 1);
      removeChildAtIndexInPlace(i - 1);
      /* CAUTION: we removed a child. So we need to decrement i to make sure
       * the next iteration is actually on the next child. */
      i--;
      nbChildren--;
      Subtraction s = Subtraction::Builder(leftSibling, subtractant);
      /* We stole subtractant from this which replaced it by a ghost. We thus
       * need to put the subtraction at the previous index of subtractant, which
       * is still i because we updated i after removing a child. */
      replaceChildAtIndexInPlace(i, s);
    }
  }

  OExpression result = squashUnaryHierarchyInPlace();
  return result;
}

OExpression Addition::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  // Step 0: Let's remove the addition if it has a single child
  assert(numberOfChildren() > 0);
  if (numberOfChildren() == 1) {
    return squashUnaryHierarchyInPlace();
  }

  /* Step 1: Addition is associative, so let's start by merging children which
   * are additions.
   * If the parent is also an addition, escape and let the parent handle the
   * reduction.
   * Doing this makes it so we reduce the whole addition at once which avoid
   * some reduction errors.
   * (These errors where namely due to child addition being factorized on same
   * denominator before being reduced with the other terms of parent addition.)
   * */
  mergeSameTypeChildrenInPlace();
  OExpression parentOfThis = parent();
  while (!parentOfThis.isUninitialized() &&
         parentOfThis.otype() == ExpressionNode::Type::Parenthesis) {
    parentOfThis = parentOfThis.parent();
  }
  if (!parentOfThis.isUninitialized() &&
      parentOfThis.isOfType({ExpressionNode::Type::Addition,
                             ExpressionNode::Type::Subtraction})) {
    return *this;
  }

  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);

  // Step 2: Sort the children
  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, true);
      },
      reductionContext.context(), reductionContext.shouldCheckMatrices());

  // Step 3 : Distribute the addition over lists
  OExpression distributed = SimplificationHelper::distributeReductionOverLists(
      *this, reductionContext);
  if (!distributed.isUninitialized()) {
    return distributed;
  }

  /* Step 4: Handle the units. All children should have the same unit, otherwise
   * the result is not homogeneous. */
  {
    OExpression unit;
    if (childAtIndex(0).removeUnit(&unit).isUndefined()) {
      return replaceWithUndefinedInPlace();
    }
    const bool hasUnit = !unit.isUninitialized();
    for (int i = 1; i < childrenCount; i++) {
      OExpression otherUnit;
      OExpression childI = childAtIndex(i).removeUnit(&otherUnit);
      if (childI.isUndefined() || hasUnit == otherUnit.isUninitialized() ||
          (hasUnit && !unit.isIdenticalTo(otherUnit))) {
        return replaceWithUndefinedInPlace();
      }
    }
    if (hasUnit) {
      /* The Tree is now free of units
       * Recurse to run the reduction, then create the result
       * result = MUL( addition, unit1, unit2...) */
      OExpression addition = shallowReduce(reductionContext);
      if (addition.isUndefined()) {
        return replaceWithUndefinedInPlace();
      }
      Multiplication result = Multiplication::Builder(unit);
      // In case `unit` was a multiplication of units, flatten
      result.mergeSameTypeChildrenInPlace();
      addition.replaceWithInPlace(result);
      result.addChildAtIndexInPlace(addition, 0, 1);
      return std::move(result);
    }
  }

  /* Step 5: Handle matrices. We return undef for a scalar added to a matrix.
   * Thanks to the simplification order, all matrix children (if any) are the
   * last children. */
  {
    OExpression lastChild = childAtIndex(childrenCount - 1);
    if (lastChild.deepIsMatrix(reductionContext.context(),
                               reductionContext.shouldCheckMatrices())) {
      if (!childAtIndex(0).deepIsMatrix(
              reductionContext.context(),
              reductionContext.shouldCheckMatrices())) {
        /* If there is a matrix in the children, the last child is a matrix. If
         * there is a a scalar, the first child is a scalar. We forbid the
         * addition of a matrix and a scalar. */
        return replaceWithUndefinedInPlace();
      }
      if (lastChild.otype() != ExpressionNode::Type::OMatrix) {
        /* All children are matrices that are not of type OMatrix (for instance
         * a ConfidenceInterval that cannot be reduced). We cannot reduce the
         * addition more. */
        return *this;
      }
      // Create the addition matrix (in place of the last child)
      OMatrix resultMatrix = static_cast<OMatrix&>(lastChild);
      int n = resultMatrix.numberOfRows();
      int m = resultMatrix.numberOfColumns();
      // Scan to add the other children, which are  matrices
      for (int i = childrenCount - 2; i >= 0; i--) {
        if (childAtIndex(i).otype() != ExpressionNode::Type::OMatrix) {
          break;
        }
        OMatrix currentMatrix = childAtIndex(i).convert<OMatrix>();
        int currentN = currentMatrix.numberOfRows();
        int currentM = currentMatrix.numberOfColumns();
        if (currentN != n || currentM != m) {
          // Addition of matrices of different dimensions -> undef
          return replaceWithUndefinedInPlace();
        }
        // Dispatch the current matrix children in the created addition matrix
        for (int j = 0; j < n * m; j++) {
          OExpression resultEntryJ = resultMatrix.childAtIndex(j);
          OExpression currentEntryJ = currentMatrix.childAtIndex(j);
          if (resultEntryJ.otype() == ExpressionNode::Type::Addition) {
            static_cast<Addition&>(resultEntryJ)
                .addChildAtIndexInPlace(currentEntryJ,
                                        resultEntryJ.numberOfChildren(),
                                        resultEntryJ.numberOfChildren());
          } else {
            Addition a = Addition::Builder(resultEntryJ, currentEntryJ);
            resultMatrix.replaceChildAtIndexInPlace(j, a);
          }
        }
        removeChildInPlace(currentMatrix, currentMatrix.numberOfChildren());
      }
      for (int j = 0; j < n * m; j++) {
        resultMatrix.childAtIndex(j).shallowReduce(reductionContext);
      }
      return squashUnaryHierarchyInPlace();
    }
  }

  /* Step 6: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  int i = 0;
  while (i < numberOfChildren() - 1) {
    OExpression e1 = childAtIndex(i);
    OExpression e2 = childAtIndex(i + 1);
    if (e1.isNumber() && e2.isNumber()) {
      Number r1 = static_cast<Number&>(e1);
      Number r2 = static_cast<Number&>(e2);
      Number a = Number::Addition(r1, r2);
      replaceChildAtIndexInPlace(i, a);
      removeChildAtIndexInPlace(i + 1);
      continue;
    }
    if (TermsHaveIdenticalNonNumeralFactors(e1, e2,
                                            reductionContext.context())) {
      factorizeChildrenAtIndexesInPlace(i, i + 1, reductionContext);
      continue;
    }
    i++;
  }

  // Step 6.2: factorize sin^2+cos^2
  for (int i = 0; i < numberOfChildren(); i++) {
    OExpression baseOfSquaredCos;
    // Find y*cos^2(x)
    if (TermHasSquaredCos(childAtIndex(i), reductionContext,
                          baseOfSquaredCos)) {
      // Try to find y*sin^2(x) and turn sum into y
      OExpression additionWithFactorizedSumOfSquaredTrigFunction =
          factorizeSquaredTrigFunction(baseOfSquaredCos, reductionContext);
      if (!additionWithFactorizedSumOfSquaredTrigFunction.isUninitialized()) {
        // If it's initialized, it means that the pattern was found
        return additionWithFactorizedSumOfSquaredTrigFunction;
      }
    }
  }

  // Factorizing terms might have created dependencies.
  OExpression eBubbledUp =
      SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!eBubbledUp.isUninitialized()) {
    // bubbleUpDependencies shallowReduces the expression
    return eBubbledUp;
  }

  /* Step 7: Let's remove any zero. It's important to do this after having
   * factorized because factorization can lead to new zeroes. For example
   * pi+(-1)*pi. We don't remove the last zero if it's the only child left
   * though. */
  i = 0;
  while (i < numberOfChildren()) {
    OExpression e = childAtIndex(i);
    if (e.otype() == ExpressionNode::Type::Rational &&
        static_cast<Rational&>(e).isZero() && numberOfChildren() > 1) {
      removeChildAtIndexInPlace(i);
      continue;
    }
    i++;
  }

  // Step 8: Let's remove the addition altogether if it has a single child
  OExpression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Step 9: Let's bubble up the complex cartesian if possible. */
  OExpression complexCombined =
      combineComplexCartesians(&ComplexCartesian::add, reductionContext);
  if (!complexCombined.isUninitialized()) {
    return complexCombined;
  }

  /* Step 10: Let's put everything under a common denominator.
   * This step is done only for ReductionTarget::User if the parent expression
   * is not an addition. */
  OExpression p = result.parent();
  if (reductionContext.target() == ReductionTarget::User && result == *this &&
      (p.isUninitialized() || p.otype() != ExpressionNode::Type::Addition)) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
    result = factorizeOnCommonDenominator(reductionContext);
  }
  return result;
}

bool Addition::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  {
    OExpression e =
        Derivative::DefaultDerivate(*this, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return true;
    }
  }

  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    derivateChildAtIndexInPlace(i, reductionContext, symbol, symbolValue);
  }
  return true;
}

int Addition::NumberOfNonNumeralFactors(const OExpression& e) {
  if (e.otype() != ExpressionNode::Type::Multiplication) {
    return 1;  // Or (e->otype() != Type::Rational);
  }
  int result = e.numberOfChildren();
  if (e.childAtIndex(0).isNumber()) {
    result--;
  }
  return result;
}

const OExpression Addition::FirstNonNumeralFactor(const OExpression& e) {
  if (e.otype() != ExpressionNode::Type::Multiplication) {
    return e;
  }
  if (e.childAtIndex(0).isNumber()) {
    return e.numberOfChildren() > 1 ? e.childAtIndex(1) : OExpression();
  }
  return e.childAtIndex(0);
}

bool Addition::TermsHaveIdenticalNonNumeralFactors(const OExpression& e1,
                                                   const OExpression& e2,
                                                   Context* context) {
  /* Return true if two expressions differ only by a rational factor. For
   * example, 2*pi and pi do, 2*pi and 2*ln(2) don't. We do not want to
   * factorize random(). */

  int numberOfNonNumeralFactorsInE1 = NumberOfNonNumeralFactors(e1);
  int numberOfNonNumeralFactorsInE2 = NumberOfNonNumeralFactors(e2);

  if (numberOfNonNumeralFactorsInE1 != numberOfNonNumeralFactorsInE2) {
    return false;
  }

  int numberOfNonNumeralFactors = numberOfNonNumeralFactorsInE1;
  if (numberOfNonNumeralFactors == 1) {
    OExpression nonNumeralFactor = FirstNonNumeralFactor(e1);
    if (nonNumeralFactor.recursivelyMatches(OExpression::IsRandom, context)) {
      return false;
    }
    return FirstNonNumeralFactor(e1).isIdenticalTo(FirstNonNumeralFactor(e2));
  } else {
    assert(numberOfNonNumeralFactors > 1);
    return Multiplication::HaveSameNonNumeralFactors(e1, e2);
  }
}

OExpression Addition::factorizeOnCommonDenominator(
    ReductionContext reductionContext) {
  /* We want to turn (a/b+c/d+e/b) into (a*d+b*c+e*d)/(b*d), except if one of
   * the denominators contains random, in which case the factors with random
   * should stay appart. */

  /* a will store the random factors, which should not be put to the same
   * denominator. */
  Addition a = Addition::Builder();

  // Step 1: We want to compute the common denominator, b*d
  Multiplication commonDenominator = Multiplication::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    OExpression childI = childAtIndex(i);
    OExpression currentDenominator = childI.denominator(reductionContext);
    if (!currentDenominator.isUninitialized()) {
      if (currentDenominator.recursivelyMatches(OExpression::IsRandom,
                                                reductionContext.context())) {
        // Remove "random" factors
        removeChildInPlace(childI, childI.numberOfChildren());
        a.addChildAtIndexInPlace(childI, a.numberOfChildren(),
                                 a.numberOfChildren());
        i--;
        continue;
      }
      /* Make commonDenominator = LeastCommonMultiple(commonDenominator,
       * denominator);
       * TODO: I'm almost certain that we do not need to create dependencies
       * when creating common denominator since we're working on reduced
       * expression. Indeed, there should not be denominators like x*x^(-1)
       * or sqrt(x)*sqrt(x). But i'm not 100% sure, so if a case is found, a
       * dependency list can be passed to "addMissingFactor", which will then
       * be passed to "Multiplication::mergeInChildByFactorizingBase" */
      commonDenominator.addMissingFactors(currentDenominator, reductionContext);
    }
  }
  if (commonDenominator.numberOfChildren() == 0 ||
      commonDenominator.recursivelyMatches(OExpression::IsUndefined,
                                           reductionContext.context())) {
    /* If commonDenominator is empty this means that no child was a fraction.
     * commonDenominator can have an undef child when reducing with Infinity
     * expression. For example, 1^(-inf) is not reduced, so factorising
     * 1^(-inf) + 10 will give (1^inf * 1^(-inf) + 1^inf * 10) / 1^(-inf)
     * and 1^(inf-inf) will be undef.*/
    while (a.numberOfChildren() > 0) {
      // Put back the "random" children
      addChildAtIndexInPlace(a.childAtIndex(0), numberOfChildren(),
                             numberOfChildren());
    }
    return *this;
  }

  /* Step 2: Create the numerator. We start with this being a/b+c/d+e/b and we
   * want to create numerator = a/b*b*d + c/d*b*d + e/b*b*d = a*d + c*b + e*d */
  // Else, before, the algorithm used User target -> put back ?
  assert(reductionContext.target() == ReductionTarget::User);
  Addition numerator = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    Multiplication m =
        Multiplication::Builder(childAtIndex(i), commonDenominator.clone());
    numerator.addChildAtIndexInPlace(m, numerator.numberOfChildren(),
                                     numerator.numberOfChildren());
    OExpression reducedM = m.shallowReduce(reductionContext);
    if (reducedM.otype() == ExpressionNode::Type::Dependency) {
      /* The reduction of the numerator might have created dependencies that
       * we do not need since the information of the forbidden values is either
       * contained in the denominator or already bubbled up.
       * Example: When creating the numerator for 3+(1/x^2), we compute
       * x^2*(3+(1/x^2)) which create a dependency in 1/x^2.
       * This is useless since the final result is (3x^2+1)/x^2 */
      reducedM.replaceWithInPlace(reducedM.childAtIndex(0));
    }
  }

  // Step 3: Add the denominator
  Power inverseDenominator =
      Power::Builder(commonDenominator, Rational::Builder(-1));
  Multiplication result =
      Multiplication::Builder(numerator, inverseDenominator);

  /* To simplify the numerator and the denominator, we temporarily disable
   * symbolic computation: all unwanted symbols should have already disappeared
   * by now, and if we checked again for symbols we might find "parameter"
   * symbols disconnected from the parametered expression (which is no longer a
   * parent here), which would be replaced with undef.
   * Example: int((e^(-x))-x^(0.5), x, 0, 3), when creating the common
   * denominator for the integrand. */
  SymbolicComputation previousSymbolicComputation =
      reductionContext.symbolicComputation();
  reductionContext.setSymbolicComputation(
      SymbolicComputation::DoNotReplaceAnySymbol);

  // Step 4: Simplify the numerator
  numerator.shallowReduce(reductionContext);

  // Step 5: Simplify the denominator (in case it's a rational number)
  inverseDenominator.deepReduce(reductionContext);

  // Restore symbolicComputation status
  reductionContext.setSymbolicComputation(previousSymbolicComputation);

  /* Step 6: We simplify the resulting multiplication forbidding any
   * distribution of multiplication on additions (to avoid an infinite loop).
   * Handle the removed random factors. */
  reductionContext.setExpandMultiplication(false);
  int aChildrenCount = a.numberOfChildren();
  if (aChildrenCount == 0) {
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }
  a.addChildAtIndexInPlace(result, aChildrenCount, aChildrenCount);
  result.shallowReduce(reductionContext);
  replaceWithInPlace(a);
  return std::move(a);
}

void Addition::factorizeChildrenAtIndexesInPlace(
    int index1, int index2, const ReductionContext& reductionContext) {
  /* This function factorizes two children which only differ by a rational
   * factor. For example, if this is AdditionNode(2*pi, 3*pi), then 2*pi and
   * 3*pi could be merged, and this turned into AdditionNode(5*pi). */
  assert(index1 >= 0 && index1 < numberOfChildren());
  assert(index2 >= 0 && index2 < numberOfChildren());

  OExpression e1 = childAtIndex(index1);
  OExpression e2 = childAtIndex(index2);

  // Step 1: Compute the new numeral factor
  Number r = Number::Addition(NumeralFactor(e1), NumeralFactor(e2));

  // Step 2: Get rid of one of the children
  removeChildAtIndexInPlace(index2);

  // Step 3: Create a multiplication
  Multiplication m = Multiplication::Builder();
  if (e1.otype() == ExpressionNode::Type::Multiplication) {
    m = static_cast<Multiplication&>(e1);
  } else {
    replaceChildAtIndexInPlace(index1, m);
    m.addChildAtIndexInPlace(e1, 0, 0);
  }

  // Step 4: Use the new rational factor
  assert(m.numberOfChildren() > 0);
  if (m.childAtIndex(0).isNumber()) {
    /* The children were ordered before, so ant rational child of m would be on
     * the first position */
    m.replaceChildAtIndexInPlace(0, r);
  } else {
    m.addChildAtIndexInPlace(r, 0, m.numberOfChildren());
  }

  // Step 5: Reduce the multiplication (in case the new rational factor is zero)
  m.shallowReduce(reductionContext);
}

bool Addition::TermHasSquaredCos(const OExpression& e,
                                 const ReductionContext& reductionContext,
                                 OExpression& baseOfCos) {
  bool isCosine;
  OExpression temp;
  bool isSquaredTrigFunction = TermHasSquaredTrigFunctionWithBase(
      e, reductionContext, baseOfCos, temp, &isCosine);
  return isSquaredTrigFunction && isCosine;
}

bool Addition::TermHasSquaredTrigFunctionWithBase(
    const OExpression& e, const ReductionContext& reductionContext,
    OExpression& base, OExpression& coefficient, bool* cosine) {
  if (e.otype() == ExpressionNode::Type::Power &&
      e.childAtIndex(0).isOfType(
          {ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine}) &&
      e.childAtIndex(1).isIdenticalTo(Rational::Builder(2))) {
    *cosine = e.childAtIndex(0).otype() == ExpressionNode::Type::Cosine;
    coefficient = Rational::Builder(1);
    OExpression trigFunctionBase = e.childAtIndex(0).childAtIndex(0);
    if (base.isUninitialized()) {
      base = trigFunctionBase.clone();
      return true;
    } else if (base.isIdenticalTo(trigFunctionBase)) {
      return true;
    }
  } else if (e.otype() == ExpressionNode::Type::Multiplication) {
    OExpression trigFunctionCoeff;
    int nChildren = e.numberOfChildren();
    for (int i = 0; i < nChildren; i++) {
      if (TermHasSquaredTrigFunctionWithBase(e.childAtIndex(i),
                                             reductionContext, base,
                                             trigFunctionCoeff, cosine)) {
        coefficient = e.clone();
        coefficient.replaceChildAtIndexInPlace(i, trigFunctionCoeff);
        coefficient = coefficient.shallowReduce(reductionContext);
        return true;
      }
    }
  }
  return false;
}

OExpression Addition::factorizeSquaredTrigFunction(
    OExpression& baseOfTrigFunction, const ReductionContext& reductionContext) {
  Addition totalCoefOfSine = Addition::Builder();
  Addition totalCoefOfCosine = Addition::Builder();
  OExpression thisClone = clone();
  assert(thisClone.otype() == ExpressionNode::Type::Addition);
  Addition result = static_cast<Addition&>(thisClone);
  for (int i = 0; i < result.numberOfChildren(); i++) {
    OExpression child = result.childAtIndex(i);
    bool isCosine;
    OExpression coefficientToAdd;
    if (TermHasSquaredTrigFunctionWithBase(child, reductionContext,
                                           baseOfTrigFunction, coefficientToAdd,
                                           &isCosine)) {
      if (isCosine) {
        totalCoefOfCosine.addChildAtIndexInPlace(
            coefficientToAdd, totalCoefOfCosine.numberOfChildren(),
            totalCoefOfCosine.numberOfChildren());
      } else {
        totalCoefOfSine.addChildAtIndexInPlace(
            coefficientToAdd, totalCoefOfSine.numberOfChildren(),
            totalCoefOfSine.numberOfChildren());
      }
      result.removeChildAtIndexInPlace(i);
      i--;
    }
  }
  if (totalCoefOfSine.numberOfChildren() == 0 ||
      totalCoefOfCosine.numberOfChildren() == 0) {
    return OExpression();
  }
  OExpression totalCoefOfSineReduced =
      totalCoefOfSine.shallowReduce(reductionContext);
  OExpression totalCoefOfCosineReduced =
      totalCoefOfCosine.shallowReduce(reductionContext);
  if (totalCoefOfCosineReduced.isIdenticalTo(totalCoefOfSineReduced)) {
    /* Replace cos(x)^2+sin(x)^2 with 1. This is true only if x is defined
     * so we add a dependency */
    result.addChildAtIndexInPlace(totalCoefOfCosineReduced,
                                  result.numberOfChildren(),
                                  result.numberOfChildren());
    Dependency dep = Dependency::Builder(result.shallowReduce(reductionContext),
                                         OList::Builder());
    dep.addDependency(baseOfTrigFunction);
    replaceWithInPlace(dep);
    return dep.shallowReduce(reductionContext);
  }
  return OExpression();
}

}  // namespace Poincare
