#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/undefined.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

Expression::Type Addition::type() const {
  return Type::Addition;
}

Expression * Addition::clone() const {
  if (numberOfOperands() == 0) {
    return new Addition();
  }
  return new Addition(operands(), numberOfOperands(), true);
}

int Addition::polynomialDegree(char symbolName) const {
  int degree = 0;
  for (int i = 0; i < numberOfOperands(); i++) {
    int d = operand(i)->polynomialDegree(symbolName);
    if (d < 0) {
      return -1;
    }
    degree = d > degree ? d : degree;
  }
  return degree;
}

/* Layout */

bool Addition::needParenthesisWithParent(const Expression * e) const {
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 6);
}

/* Simplication */

Expression * Addition::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  /* Step 1: Addition is associative, so let's start by merging children which
   * also are additions themselves. */
  int i = 0;
  int initialNumberOfOperands = numberOfOperands();
  while (i < initialNumberOfOperands) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Addition) {
      mergeOperands(static_cast<Addition *>(o));
      continue;
    }
    i++;
  }

  // Step 2: Sort the operands
  sortOperands(Expression::SimplificationOrder, true);

#if MATRIX_EXACT_REDUCING
  /* Step 2bis: get rid of matrix */
  int n = 1;
  int m = 1;
  /* All operands have been simplified so if any operand contains a matrix, it
   * is at the root node of the operand. Moreover, thanks to the simplification
   * order, all matrix operands (if any) are the last operands. */
  Expression * lastOperand = editableOperand(numberOfOperands()-1);
  if (lastOperand->type() == Type::Matrix) {
    // Create in-place the matrix of addition M (in place of the last operand)
    Matrix * resultMatrix = static_cast<Matrix *>(lastOperand);
    n = resultMatrix->numberOfRows();
    m = resultMatrix->numberOfColumns();
    removeOperand(resultMatrix, false);
    /* Scan (starting at the end) accross the addition operands to find any
     * other matrix */
    int i = numberOfOperands()-1;
    while (i >= 0 && operand(i)->type() == Type::Matrix) {
      Matrix * currentMatrix = static_cast<Matrix *>(editableOperand(i));
      int on = currentMatrix->numberOfRows();
      int om = currentMatrix->numberOfColumns();
      if (on != n || om != m) {
        return replaceWith(new Undefined(), true);
      }
      // Dispatch the current matrix operands in the created additions matrix
      for (int j = 0; j < n*m; j++) {
        Addition * a = new Addition();
        Expression * resultMatrixEntryJ = resultMatrix->editableOperand(j);
        resultMatrix->replaceOperand(resultMatrixEntryJ, a, false);
        a->addOperand(currentMatrix->editableOperand(j));
        a->addOperand(resultMatrixEntryJ);
        a->shallowReduce(context, angleUnit);
      }
      currentMatrix->detachOperands();
      removeOperand(currentMatrix, true);
      i--;
    }
    // Distribute the remaining addition on matrix operands
    for (int i = 0; i < n*m; i++) {
      Addition * a = static_cast<Addition *>(clone());
      Expression * entryI = resultMatrix->editableOperand(i);
      resultMatrix->replaceOperand(entryI, a, false);
      a->addOperand(entryI);
      a->shallowReduce(context, angleUnit);
    }
    return replaceWith(resultMatrix, true)->shallowReduce(context, angleUnit);
  }
#endif

  /* Step 3: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  i = 0;
  while (i < numberOfOperands()-1) {
    Expression * o1 = editableOperand(i);
    Expression * o2 = editableOperand(i+1);
    if (o1->type() == Type::Rational && o2->type() == Type::Rational) {
      Rational r1 = *static_cast<Rational *>(o1);
      Rational r2 = *static_cast<Rational *>(o2);
      Rational a = Rational::Addition(r1, r2);
      replaceOperand(o1, new Rational(a), true);
      removeOperand(o2, true);
      continue;
    }
    if (TermsHaveIdenticalNonRationalFactors(o1, o2)) {
      factorizeOperands(o1, o2, context, angleUnit);
      continue;
    }
    i++;
  }

  /* Step 4: Let's remove zeroes if there's any. It's important to do this after
   * having factorized because factorization can lead to new zeroes. For example
   * pi+(-1)*pi. We don't remove the last zero if it's the only operand left
   * though. */
  i = 0;
  while (i < numberOfOperands()) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Rational && static_cast<Rational *>(o)->isZero() && numberOfOperands() > 1) {
      removeOperand(o, true);
      continue;
    }
    i++;
  }

  // Step 5: Let's remove the addition altogether if it has a single operand
  Expression * result = squashUnaryHierarchy();

  // Step 6: Last but not least, let's put everything under a common denominator
  if (result == this && parent()->type() != Type::Addition) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
    result = factorizeOnCommonDenominator(context, angleUnit);
  }

  return result;
}

Expression * Addition::factorizeOnCommonDenominator(Context & context, AngleUnit angleUnit) {
  // We want to turn (a/b+c/d+e/b) into (a*d+b*c+e*d)/(b*d)

  // Step 1: We want to compute the common denominator, b*d
  Multiplication * commonDenominator = new Multiplication();
  for (int i = 0; i < numberOfOperands(); i++) {
    Expression * denominator = operand(i)->cloneDenominator(context, angleUnit);
    if (denominator) {
      // Make commonDenominator = LeastCommonMultiple(commonDenominator, denominator);
      commonDenominator->addMissingFactors(denominator, context, angleUnit);
      delete denominator;
    }
  }
  if (commonDenominator->numberOfOperands() == 0) {
    delete commonDenominator;
    // If commonDenominator is empty this means that no operand was a fraction.
    return this;
  }

  // Step 2: Create the numerator. We start with this being a/b+c/d+e/b and we
  // want to create numerator = a/b*b*d + c/d*b*d + e/b*b*d
  Addition * numerator = new Addition();
  for (int i=0; i < numberOfOperands(); i++) {
    Multiplication * m = new Multiplication(operand(i), commonDenominator, true);
    numerator->addOperand(m);
    m->privateShallowReduce(context, angleUnit, true, false);
  }
  // Step 3: Add the denominator
  Power * inverseDenominator = new Power(commonDenominator, new Rational(-1), false);
  Multiplication * result = new Multiplication(numerator, inverseDenominator, false);

  // Step 4: Simplify the numerator to a*d + c*b + e*d
  numerator->shallowReduce(context, angleUnit);

  // Step 5: Simplify the denominator (in case it's a rational number)
  commonDenominator->deepReduce(context, angleUnit);
  inverseDenominator->shallowReduce(context, angleUnit);

  /* Step 6: We simplify the resulting multiplication forbidding any
   * distribution of multiplication on additions (to avoid an infinite loop). */
  return static_cast<Multiplication *>(replaceWith(result, true))->privateShallowReduce(context, angleUnit, false, true);
}

void Addition::factorizeOperands(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit) {
  /* This function factorizes two operands which only differ by a rational
   * factor. For example, if this is Addition(2*pi, 3*pi), then 2*pi and 3*pi
   * could be merged, and this turned into Addition(5*pi). */
  assert(e1->parent() == this && e2->parent() == this);

  // Step 1: Find the new rational factor
  Rational * r = new Rational(Rational::Addition(RationalFactor(e1), RationalFactor(e2)));

  // Step 2: Get rid of one of the operands
  removeOperand(e2, true);

  // Step 3: Use the new rational factor. Create a multiplication if needed
  Multiplication * m = nullptr;
  if (e1->type() == Type::Multiplication) {
    m = static_cast<Multiplication *>(e1);
  } else {
    m = new Multiplication();
    e1->replaceWith(m, false);
    m->addOperand(e1);
  }
  if (m->operand(0)->type() == Type::Rational) {
    m->replaceOperand(m->operand(0), r, true);
  } else {
    m->addOperand(r);
  }

  // Step 4: Reduce the multiplication (in case the new rational factor is zero)
  m->shallowReduce(context, angleUnit);
}

const Rational Addition::RationalFactor(Expression * e) {
  if (e->type() == Type::Multiplication && e->operand(0)->type() == Type::Rational) {
    return *(static_cast<const Rational *>(e->operand(0)));
  }
  return Rational(1);
}

static inline int NumberOfNonRationalFactors(const Expression * e) {
  if (e->type() != Expression::Type::Multiplication) {
    return 1; // Or (e->type() != Type::Rational);
  }
  int result = e->numberOfOperands();
  if (e->operand(0)->type() == Expression::Type::Rational) {
    result--;
  }
  return result;
}

static inline const Expression * FirstNonRationalFactor(const Expression * e) {
  if (e->type() != Expression::Type::Multiplication) {
    return e;
  }
  if (e->operand(0)->type() == Expression::Type::Rational) {
    return e->numberOfOperands() > 1 ? e->operand(1) : nullptr;
  }
  return e->operand(0);
}

bool Addition::TermsHaveIdenticalNonRationalFactors(const Expression * e1, const Expression * e2) {
  /* Given two expressions, say wether they only differ by a rational factor.
   * For example, 2*pi and pi do, 2*pi and 2*ln(2) don't. */

  int numberOfNonRationalFactorsInE1 = NumberOfNonRationalFactors(e1);
  int numberOfNonRationalFactorsInE2 = NumberOfNonRationalFactors(e2);

  if (numberOfNonRationalFactorsInE1 != numberOfNonRationalFactorsInE2) {
    return false;
  }

  int numberOfNonRationalFactors = numberOfNonRationalFactorsInE1;
  if (numberOfNonRationalFactors == 1) {
    return FirstNonRationalFactor(e1)->isIdenticalTo(FirstNonRationalFactor(e2));
  } else {
    assert(numberOfNonRationalFactors > 1);
    return Multiplication::HaveSameNonRationalFactors(e1, e2);
  }
}

Expression * Addition::shallowBeautify(Context & context, AngleUnit angleUnit) {
  /* Beautifying Addition essentially consists in adding Subtractions if needed.
   * In practice, we want to turn "a+(-1)*b" into "a-b". Or, more precisely, any
   * "a+(-r)*b" into "a-r*b" where r is a positive Rational.
   * Note: the process will slightly differ if the negative product occurs on
   * the first term: we want to turn "Addition(Multiplication(-1,b))" into
   * "Opposite(b)".
   * Last but not least, special care must be taken when iterating over operands
   * since we may remove some during the process. */

  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i)->type() != Type::Multiplication || operand(i)->operand(0)->type() != Type::Rational || operand(i)->operand(0)->sign() != Sign::Negative) {
      // Ignore terms which are not like "(-r)*a"
      continue;
    }

    Multiplication * m = static_cast<Multiplication *>(editableOperand(i));

    if (static_cast<const Rational *>(m->operand(0))->isMinusOne()) {
      m->removeOperand(m->operand(0), true);
    } else {
      m->editableOperand(0)->setSign(Sign::Positive, context, angleUnit);
    }
    Expression * subtractant = m->squashUnaryHierarchy();

    if (i == 0) {
      Opposite * o = new Opposite(subtractant, true);
      replaceOperand(subtractant, o, true);
    } else {
      const Expression * op1 = operand(i-1);
      removeOperand(op1, false);
      Subtraction * s = new Subtraction(op1, subtractant, false);
      replaceOperand(subtractant, s, false);
      /* CAUTION: we removed an operand. So we need to decrement i to make sure
       * the next iteration is actually on the next operand. */
      i--;
    }
  }

  return squashUnaryHierarchy();
}

/* Evaluation */

template<typename T>
Complex<T> Addition::compute(const Complex<T> c, const Complex<T> d) {
  return Complex<T>::Cartesian(c.a()+d.a(), c.b()+d.b());
}

template Complex<float> Poincare::Addition::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Complex<double> Poincare::Addition::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);

template Matrix* Addition::computeOnMatrices<float>(const Matrix*,const Matrix*);
template Matrix* Addition::computeOnMatrices<double>(const Matrix*,const Matrix*);

template Matrix* Addition::computeOnComplexAndMatrix<float>(Complex<float> const*, const Matrix*);
template Matrix* Addition::computeOnComplexAndMatrix<double>(Complex<double> const*, const Matrix*);

}
