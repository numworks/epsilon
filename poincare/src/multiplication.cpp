extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <poincare/complex_matrix.h>
#include <poincare/undefined.h>
#include <poincare/parenthesis.h>
#include <poincare/subtraction.h>
#include <poincare/division.h>
#include <poincare/arithmetic.h>
#include <poincare/simplification_root.h>
#include <ion.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression::Type Multiplication::type() const {
  return Expression::Type::Multiplication;
}

Expression * Multiplication::clone() const {
  return new Multiplication(operands(), numberOfOperands(), true);
}

ExpressionLayout * Multiplication::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  const char middleDotString[] = {Ion::Charset::MiddleDot, 0};
  return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, middleDotString);
}

int Multiplication::writeTextInBuffer(char * buffer, int bufferSize) const {
  const char multiplicationString[] = {Ion::Charset::MultiplicationSign, 0};
  return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, multiplicationString);
}

Expression::Sign Multiplication::sign() const {
  int sign = 1;
  for (int i = 0; i < numberOfOperands(); i++) {
    sign *= (int)operand(i)->sign();
  }
  return (Sign)sign;
}

Expression * Multiplication::setSign(Sign s, Context & context, AngleUnit angleUnit) {
  assert(s == Sign::Positive);
  for (int i = 0; i < numberOfOperands(); i++) {
    if (operand(i)->sign() == Sign::Negative) {
      editableOperand(i)->setSign(s, context, angleUnit);
    }
  }
  return shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> Multiplication::compute(const Complex<T> c, const Complex<T> d) {
  return Complex<T>::Cartesian(c.a()*d.a()-c.b()*d.b(), c.b()*d.a() + c.a()*d.b());
}

template<typename T>
Evaluation<T> * Multiplication::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
  if (m->numberOfColumns() != n->numberOfRows()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*n->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows(); i++) {
    for (int j = 0; j < n->numberOfColumns(); j++) {
      T a = 0.0f;
      T b = 0.0f;
      for (int k = 0; k < m->numberOfColumns(); k++) {
        Complex<T> mEntry = *(m->complexOperand(i*m->numberOfColumns()+k));
        Complex<T> nEntry = *(n->complexOperand(k*n->numberOfColumns()+j));
        a += mEntry.a()*nEntry.a() - mEntry.b()*nEntry.b();
        b += mEntry.b()*nEntry.a() + mEntry.a()*nEntry.b();
      }
      operands[i*n->numberOfColumns()+j] = Complex<T>::Cartesian(a, b);
    }
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), n->numberOfColumns());
  delete[] operands;
  return result;
}

bool Multiplication::HaveSameNonRationalFactors(const Expression * e1, const Expression * e2) {
  int numberOfNonRationalFactors1 = e1->operand(0)->type() == Type::Rational ? e1->numberOfOperands()-1 : e1->numberOfOperands();
  int numberOfNonRationalFactors2 = e2->operand(0)->type() == Type::Rational ? e2->numberOfOperands()-1 : e2->numberOfOperands();
  if (numberOfNonRationalFactors1 != numberOfNonRationalFactors2) {
    return false;
  }
  int firstNonRationalOperand1 = e1->operand(0)->type() == Type::Rational ? 1 : 0;
  int firstNonRationalOperand2 = e2->operand(0)->type() == Type::Rational ? 1 : 0;
  for (int i = 0; i < numberOfNonRationalFactors1; i++) {
    if (!(e1->operand(firstNonRationalOperand1+i)->isIdenticalTo(e2->operand(firstNonRationalOperand2+i)))) {
      return false;
    }
  }
  return true;
}

Expression * Multiplication::shallowReduce(Context& context, AngleUnit angleUnit) {
  /* Step 1: Multiplication is associative, so let's start by merging children
   * which also are additions themselves. */
  int i = 0;
  int initialNumberOfOperands = numberOfOperands();
  while (i < initialNumberOfOperands) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Multiplication) {
      mergeOperands(static_cast<Multiplication *>(o));
      continue;
    }
    i++;
  }
  /* Step 2: If any of the operand is zero, the multiplication result is zero */
  for (int i = 0; i < numberOfOperands(); i++) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Rational && static_cast<const Rational *>(o)->isZero()) {
      return replaceWith(new Rational(0), true);
    }
  }
  // Step 3: Sort the operands
  sortOperands(SimplificationOrder);

  /* Step 4: Factorize like terms before expanding multiplication of addition.
   * This step enables to reduce expressions like (x+y)^(-1)*(x+y)(a+b) for
   * example. Thanks to the simplification order, those are next to each other
   * at this point. */
  i = 0;
  while (i < numberOfOperands()-1) {
    if (operand(i)->type() == Type::Rational && operand(i+1)->type() == Type::Rational) {
      Rational a = Rational::Multiplication(*(static_cast<const Rational *>(operand(i))), *(static_cast<const Rational *>(operand(i+1))));
      replaceOperand(operand(i), new Rational(a), true);
      removeOperand(operand(i+1), true);
      continue;
    } else if (TermsHaveIdenticalBase(operand(i), operand(i+1)) && (!TermHasRationalBase(operand(i)) || (!TermHasIntegerExponent(operand(i)) && !TermHasIntegerExponent(operand(i+1))))) {
      factorizeBase(editableOperand(i), editableOperand(i+1), context, angleUnit);
      continue;
    } else if (TermsHaveIdenticalNonUnitaryExponent(operand(i), operand(i+1)) && TermHasRationalBase(operand(i)) && TermHasRationalBase(operand(i+1))) {
      factorizeExponent(editableOperand(i), editableOperand(i+1), context, angleUnit);
      continue;
    }
    i++;
  }
  /* Step 5: Let's remove ones if there's any. It's important to do this after
   * having factorized because factorization can lead to new ones. For example
   * pi^(-1)*pi. We don't remove the last one if it's the only operand left
   * though. */
  i = 0;
  while (i < numberOfOperands()) {
    Expression * o = editableOperand(i);
    if (o->type() == Type::Rational && static_cast<Rational *>(o)->isOne() && numberOfOperands() > 1) {
      removeOperand(o, true);
      continue;
    }
    i++;
  }
  /* Step 6: we distribute multiplication on addition node. We do not want to
   * do this step if the parent is a multiplication to avoid missing
   * factorization like (x+y)^(-1)*((a+b)*(x+y) */
  if (parent()->type() != Type::Multiplication) {
    for (int i=0; i<numberOfOperands(); i++) {
      if (operand(i)->type() == Type::Addition) {
        return distributeOnOperandAtIndex(i, context, angleUnit);
      }
    }
  }
  /* Step 7: Let's remove the multiplication altogether if it has a single
   * operand. */
  Expression * result = squashUnaryHierarchy();
  /* Step 8: We look for square root and sum of square root (two terms maximum
   * so far) to move them at numerator. */
  if (true) {
    result = result->resolveSquareRootAtDenominator(context, angleUnit);
  }
  return result;
}

Expression * Multiplication::resolveSquareRootAtDenominator(Context & context, AngleUnit angleUnit) {
  for (int index = 0; index < numberOfOperands(); index++) {
    Expression * o = editableOperand(index);
    if (o->type() == Type::Power && o->operand(0)->type() == Type::Rational && o->operand(1)->type() == Type::Rational && static_cast<const Rational *>(o->operand(1))->isMinusHalf()) {
      Integer p = static_cast<const Rational *>(o->operand(0))->numerator();
      Integer q = static_cast<const Rational *>(o->operand(0))->denominator();
      Power * sqrt = new Power(new Rational(Integer::Multiplication(p, q)), new Rational(1, 2), false);
      replaceOperand(o, new Rational(Integer(1), p), true);
      Expression * newExpression = shallowReduce(context, angleUnit);
      if (newExpression->type() == Type::Multiplication) {
        static_cast<Multiplication *>(newExpression)->addOperand(sqrt);
      } else {
        newExpression = newExpression->replaceWith(new Multiplication(newExpression->clone(), sqrt, false), true);
      }
      sqrt->shallowReduce(context, angleUnit);
      return newExpression;
    } else if (o->type() == Type::Power && o->operand(1)->type() == Type::Rational && static_cast<const Rational *>(o->operand(1))->isMinusOne() && o->operand(0)->type() == Type::Addition && o->operand(0)->numberOfOperands() == 2 && TermIsARationalSquareRootOrRational(o->operand(0)->operand(0)) && TermIsARationalSquareRootOrRational(o->operand(0)->operand(1))) {
      const Rational * f1 = RationalFactorInExpression(o->operand(0)->operand(0));
      const Rational * f2 = RationalFactorInExpression(o->operand(0)->operand(1));
      const Rational * r1 = RadicandInExpression(o->operand(0)->operand(0));
      const Rational * r2 = RadicandInExpression(o->operand(0)->operand(1));
      Integer n1 = f1 != nullptr ? f1->numerator() : Integer(1);
      Integer d1 = f1 != nullptr ? f1->denominator() : Integer(1);
      Integer p1 = r1 != nullptr ? r1->numerator() : Integer(1);
      Integer q1 = r1 != nullptr ? r1->denominator() : Integer(1);
      Integer n2 = f2 != nullptr ? f2->numerator() : Integer(1);
      Integer d2 = f2 != nullptr ? f2->denominator() : Integer(1);
      Integer p2 = r2 != nullptr ? r2->numerator() : Integer(1);
      Integer q2 = r2 != nullptr ? r2->denominator() : Integer(1);
      // Compute n1^2*d2^2*p1*q2-n2^2*d1^2*p2*q1
      Integer denominator = Integer::Subtraction(
          Integer::Multiplication(
            Integer::Multiplication(
              Integer::Power(n1, Integer(2)),
              Integer::Power(d2, Integer(2))),
            Integer::Multiplication(p1, q2)),
          Integer::Multiplication(
            Integer::Multiplication(
              Integer::Power(n2, Integer(2)),
              Integer::Power(d1, Integer(2))),
            Integer::Multiplication(p2, q1)));
      Power * sqrt1 = new Power(new Rational(Integer::Multiplication(p1, q1)), new Rational(1, 2), false);
      Power * sqrt2 = new Power(new Rational(Integer::Multiplication(p2, q2)), new Rational(1, 2), false);
      Integer factor1 = Integer::Multiplication(
          Integer::Multiplication(n1, d1),
          Integer::Multiplication(Integer::Power(d2, Integer(2)), q2));
      Multiplication * m1 = new Multiplication(new Rational(factor1), sqrt1, false);
      Integer factor2 = Integer::Multiplication(
          Integer::Multiplication(n2, d2),
          Integer::Multiplication(Integer::Power(d1, Integer(2)), q1));
      Multiplication * m2 = new Multiplication(new Rational(factor2), sqrt2, false);
      const Expression * subOperands[2] = {m1, m2};
      if (denominator.isNegative()) {
        denominator.setNegative(false);
        const Expression * temp = subOperands[0];
        subOperands[0] = subOperands[1];
        subOperands[1] = temp;
      }
      Subtraction * s = new Subtraction(subOperands, false);
      replaceOperand(o, s, true);
      s->deepReduce(context, angleUnit);
      addOperand(new Rational(Integer(1), denominator));
      return shallowReduce(context, angleUnit);
    }
  }
  return this;
}

void Multiplication::factorizeBase(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit) {
  Expression * s = new Addition(CreateExponent(e1), CreateExponent(e2), false);
  removeOperand(e2, true);
  if (e1->type() == Type::Power) {
    e1->replaceOperand(e1->operand(1), s, true);
    s->shallowReduce(context, angleUnit);
    e1->shallowReduce(context, angleUnit);
  } else {
    Power * p = new Power(e1, s, false);
    s->shallowReduce(context, angleUnit);
    replaceOperand(e1, p, false);
    p->shallowReduce(context, angleUnit);
  }
}

void Multiplication::factorizeExponent(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit) {
  const Expression * base1 = e1->operand(0)->clone();
  const Expression * base2 = e2->operand(0);
  // TODO: remove cast, everything is a hierarchy
  static_cast<Hierarchy *>(e2)->detachOperand(base2);
  Expression * m = new Multiplication(base1, base2, false);
  removeOperand(e2, true);
  e1->replaceOperand(e1->operand(0), m, true);
  m->shallowReduce(context, angleUnit);
  e1->shallowReduce(context, angleUnit);
}

Expression * Multiplication::distributeOnOperandAtIndex(int i, Context & context, AngleUnit angleUnit) {
  Addition * a = static_cast<Addition *>(editableOperand(i));
  for (int j = 0; j < a->numberOfOperands(); j++) {
    Expression * termJ = a->editableOperand(j);
    replaceOperand(operand(i), termJ->clone(), false);
    Expression * m = clone();
    a->replaceOperand(termJ, m, true);
    m->shallowReduce(context, angleUnit);
  }
  replaceWith(a, true);
  return a->shallowReduce(context, angleUnit);
}

const Expression * Multiplication::CreateExponent(Expression * e) {
  Expression * n = e->type() == Type::Power ? e->operand(1)->clone() : new Rational(1);
  return n;
}

bool Multiplication::TermsHaveIdenticalBase(const Expression * e1, const Expression * e2) {
  const Expression * f1 = e1->type() == Type::Power ? e1->operand(0) : e1;
  const Expression * f2 = e2->type() == Type::Power ? e2->operand(0) : e2;
  return f1->isIdenticalTo(f2);
}

bool Multiplication::TermsHaveIdenticalNonUnitaryExponent(const Expression * e1, const Expression * e2) {
  return e1->type() == Type::Power && e2->type() == Type::Power && (e1->operand(1)->isIdenticalTo(e2->operand(1)));
}

bool Multiplication::TermHasRationalBase(const Expression * e) {
  bool hasRationalBase = e->type() == Type::Power ? e->operand(0)->type() == Type::Rational : e->type() == Type::Rational;
  return hasRationalBase;
}

bool Multiplication::TermHasIntegerExponent(const Expression * e) {
  if (e->type() != Type::Power) {
    return true;
  }
  if (e->operand(1)->type() == Type::Rational) {
    const Rational * r = static_cast<const Rational *>(e->operand(1));
    if (r->denominator().isOne()) {
      return true;
    }
  }
  return false;
}

Expression * Multiplication::shallowBeautify(Context & context, AngleUnit angleUnit) {
  // -1*A -> -A or (-n)*A -> -n*A
  if (operand(0)->type() == Type::Rational && operand(0)->sign() == Sign::Negative) {
    if (static_cast<const Rational *>(operand(0))->isMinusOne()) {
      removeOperand(editableOperand(0), true);
    } else {
      editableOperand(0)->setSign(Sign::Positive, context, angleUnit);
    }
    Expression * e = squashUnaryHierarchy();
    Opposite * o = new Opposite(e, true);
    e->replaceWith(o, true);
    o->editableOperand(0)->shallowBeautify(context, angleUnit);
    return o;
  }
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  Expression * e = mergeNegativePower(context, angleUnit);
  if (e->type() == Type::Power) {
    return e->shallowBeautify(context, angleUnit);
  }
  assert(e == this);
  // Add parenthesis: *(+(a,b), c) -> *((+(a,b)), c
  for (int index = 0; index < numberOfOperands(); index++) {
    // Add parenthesis to addition - (a+b)*c
    if (operand(index)->type() == Type::Addition ) {
      const Expression * o[1] = {operand(index)};
      Parenthesis * p = new Parenthesis(o, true);
      replaceOperand(operand(index), p, true);
    }
  }
  for (int index = 0; index < numberOfOperands(); index++) {
    // a*b^(-1)*... -> a*.../b
    if (operand(index)->type() == Type::Power && operand(index)->operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(index)->operand(1))->isMinusOne()) {
      Power * p = static_cast<Power *>(editableOperand(index));
      Expression * denominatorOperand = p->editableOperand(0);
      p->detachOperand(denominatorOperand);
      removeOperand(p, true);

      Expression * numeratorOperand = clone();
      Division * d = new Division(numeratorOperand, denominatorOperand, false);
      /* We want 1/3*Pi*(ln(2))^-1 -> Pi/(3ln(2)) and not ((1/3)Pi)/ln(2)*/
      if (numeratorOperand->operand(0)->type() == Type::Rational) {
        Rational * r = static_cast<Rational *>(numeratorOperand->editableOperand(0));
        if (!r->denominator().isOne()) {
          if (denominatorOperand->type() == Type::Multiplication) {
            static_cast<Multiplication *>(denominatorOperand)->addOperand(new Rational(r->denominator()));
            static_cast<Multiplication *>(denominatorOperand)->sortOperands(SimplificationOrder);
          } else {
            Multiplication * m = new Multiplication(new Rational(r->denominator()), denominatorOperand->clone(), false);
            denominatorOperand->replaceWith(m, true);
          }
        }
        if (!r->numerator().isMinusOne() || numeratorOperand->numberOfOperands() == 1) {
          numeratorOperand->replaceOperand(r, new Rational(r->numerator()), true);
          numeratorOperand = numeratorOperand->shallowReduce(context, angleUnit);
        } else {
          ((Multiplication *)numeratorOperand)->removeOperand(r, true);
          numeratorOperand = numeratorOperand->shallowReduce(context, angleUnit);
          Opposite * o = new Opposite(numeratorOperand, true);
          numeratorOperand = numeratorOperand->replaceWith(o, true);
        }
      } else {
        numeratorOperand = numeratorOperand->shallowReduce(context, angleUnit);
      }
      // Delete parenthesis unnecessary on numerator
      if (numeratorOperand->type() == Type::Parenthesis) {
        numeratorOperand->replaceWith(numeratorOperand->editableOperand(0), true);
      }
      replaceWith(d, true);
      return d->shallowBeautify(context, angleUnit);
    }
  }
  return this;
}

Expression * Multiplication::cloneDenominator(Context & context, AngleUnit angleUnit) const {
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  // WARNING: we do not want to change the expression but to create a new one.
  SimplificationRoot root(clone());
  Expression * e = ((Multiplication *)root.operand(0))->mergeNegativePower(context, angleUnit);
  Expression * result = nullptr;
  if (e->type() == Type::Power) {
    result = static_cast<Power *>(e)->cloneDenominator(context, angleUnit);
  } else {
    assert(e->type() == Type::Multiplication);
    for (int index = 0; index < e->numberOfOperands(); index++) {
      // a*b^(-1)*... -> a*.../b
      if (e->operand(index)->type() == Type::Power && e->operand(index)->operand(1)->type() == Type::Rational && static_cast<const Rational *>(e->operand(index)->operand(1))->isMinusOne()) {
        Power * p = static_cast<Power *>(e->editableOperand(index));
        result = p->editableOperand(0);
        p->detachOperand((result));
      }
    }
  }
  root.detachOperand(e);
  delete e;
  return result;
}

Expression * Multiplication::mergeNegativePower(Context & context, AngleUnit angleUnit) {
  Multiplication * m = new Multiplication();
  // Special case for rational p/q: if q != 1, q should be at denominator
  if (operand(0)->type() == Type::Rational && !static_cast<const Rational *>(operand(0))->denominator().isOne()) {
    Rational * r = static_cast<Rational *>(editableOperand(0));
    m->addOperand(new Rational(r->denominator()));
    if (r->numerator().isOne()) {
      removeOperand(r, true);
    } else {
      replaceOperand(r, new Rational(r->numerator()), true);
    }
  }
  int i = 0;
  while (i < numberOfOperands()) {
    if (operand(i)->type() == Type::Power && operand(i)->operand(1)->sign() == Sign::Negative) {
      Expression * e = editableOperand(i);
      e->editableOperand(1)->setSign(Sign::Positive, context, angleUnit);
      removeOperand(e, false);
      m->addOperand(e);
      e->shallowReduce(context, angleUnit);
    } else {
      i++;
    }
  }
  if (m->numberOfOperands() == 0) {
    return this;
  }
  Power * p = new Power(m, new Rational(-1), false);
  m->sortOperands(SimplificationOrder);
  m->squashUnaryHierarchy();
  addOperand(p);
  sortOperands(SimplificationOrder);
  return squashUnaryHierarchy();
}

void Multiplication::addMissingFactors(Expression * factor, Context & context, AngleUnit angleUnit) {
  if (factor->type() == Type::Multiplication) {
    for (int j = 0; j < factor->numberOfOperands(); j++) {
      addMissingFactors(factor->editableOperand(j), context, angleUnit);
    }
    return;
  }
  if (numberOfOperands() > 0 && operand(0)->type() == Type::Rational && factor->type() == Type::Rational) {
    Rational * f = static_cast<Rational *>(factor);
    Rational * o = static_cast<Rational *>(editableOperand(0));
    assert(f->denominator().isOne());
    assert(o->denominator().isOne());
    Integer i = f->numerator();
    Integer j = o->numerator();
    return replaceOperand(o, new Rational(Arithmetic::LCM(&i, &j)));
  }
  for (int i = 0; i < numberOfOperands(); i++) {
    if (TermsHaveIdenticalBase(operand(i), factor)) {
      Subtraction * sub = new Subtraction(CreateExponent(editableOperand(i)), CreateExponent(factor), false);
      Reduce((Expression **)&sub, context, angleUnit);
      if (sub->sign() == Sign::Negative) { // index[0] < index[1]
        factor->replaceOperand(factor->editableOperand(1), new Opposite(sub, true), true);
        factor->deepReduce(context, angleUnit);
        factorizeBase(editableOperand(i), factor, context, angleUnit);
      } else if (sub->sign() == Sign::Unknown) {
        factorizeBase(editableOperand(i), factor, context, angleUnit);
      } else {}
      delete sub;
      return;
    }
  }
  addOperand(factor->clone());
  sortOperands(SimplificationOrder);
}

template Poincare::Evaluation<float>* Poincare::Multiplication::computeOnComplexAndMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Multiplication::computeOnComplexAndMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*);
}
template Poincare::Complex<float> Poincare::Multiplication::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Poincare::Complex<double> Poincare::Multiplication::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);
