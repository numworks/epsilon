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

int Multiplication::sign() const {
  int sign = 1;
  for (int i = 0; i < numberOfOperands(); i++) {
    sign *= operand(i)->sign();
  }
  return sign;
}

void Multiplication::turnIntoPositive() {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (operand(i)->sign() < 0) {
      const_cast<Expression *>(operand(i))->turnIntoPositive();
    }
  }
  immediateSimplify();
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
    if (e1->operand(firstNonRationalOperand1+i)->compareTo(e2->operand(firstNonRationalOperand2+i)) != 0) {
      return false;
    }
  }
  return true;
}

Expression * Multiplication::immediateSimplify() {
  /* First loop: merge all multiplication, break if 0 or undef */
  int index = 0;
  /* TODO: optimize, do we have to restart index = 0 at every merging? */
  while (index < numberOfOperands()) {
    Expression * o = (Expression *)operand(index++);
    if (o->type() == Type::Multiplication) {
      mergeOperands(static_cast<Multiplication *>(o));
      index = 0;
    } else if (o->type() == Type::Rational && static_cast<const Rational *>(o)->isZero()) {
      return replaceWith(new Rational(Integer(0)), true);
    } else if (o->type() == Type::Undefined) {
      return replaceWith(new Undefined(), true);
    }
  }
  factorize();
  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i)->type() == Type::Addition) {
      return distributeOnChildAtIndex(i);
    }
  }
  /* Now, no more node can be an addition or a multiplication */
  factorize();
  return squashUnaryHierarchy();
  }

void Multiplication::factorize() {
  sortChildren();
  int i = 0;
  while (i < numberOfOperands()) {
    if (deleteUselessOperand(i) && i > 0) {
      i--;
    }
    if (i == numberOfOperands()-1) {
      break;
    }
    if (operand(i)->type() == Type::Rational && operand(i+1)->type() == Type::Rational) {
      Rational a = Rational::Multiplication(*(static_cast<const Rational *>(operand(i))), *(static_cast<const Rational *>(operand(i+1))));
      replaceOperand(operand(i), new Rational(a), true);
      removeOperand(operand(i+1), true);
    } else if (TermsHaveIdenticalBase(operand(i), operand(i+1)) && !TermHasRationalBase(operand(i))) {
      factorizeBase(const_cast<Expression *>(operand(i)), const_cast<Expression *>(operand(i+1)));
    } else if (TermsHaveIdenticalNonUnitaryExponent(operand(i), operand(i+1)) && TermHasRationalBase(operand(i)) && TermHasRationalBase(operand(i+1))) {
      factorizeExponent(const_cast<Expression *>(operand(i)), const_cast<Expression *>(operand(i+1)));
    } else {
      i++;
    }
  }
}

void Multiplication::factorizeBase(Expression * e1, Expression * e2) {
  const Expression * addOperands[2] = {CreateExponent(e1), CreateExponent(e2)};
  removeOperand(e2, true);
  Expression * s = new Addition(addOperands, 2, false);
  if (e1->type() == Type::Power) {
    e1->replaceOperand(e1->operand(1), s, true);
    s->immediateSimplify();
    e1->immediateSimplify();
  } else {
    const Expression * operands[2] = {e1, s};
    Power * p = new Power(operands, false);
    s->immediateSimplify();
    replaceOperand(e1, p, false);
    p->immediateSimplify();
  }
}

void Multiplication::factorizeExponent(Expression * e1, Expression * e2) {
  const Expression * multOperands[2] = {e1->operand(0)->clone(), e2->operand(0)};
  // TODO: remove cast, everything is a hierarchy
  static_cast<Hierarchy *>(e2)->detachOperand(e2->operand(0));
  removeOperand(e2, true);
  Expression * m = new Multiplication(multOperands, 2, false);
  e1->replaceOperand(e1->operand(0), m, true);
  m->immediateSimplify();
  e1->immediateSimplify();
}

Expression * Multiplication::distributeOnChildAtIndex(int i) {
  Addition * a = static_cast<Addition *>((Expression *) operand(i));
  for (int j = 0; j < a->numberOfOperands(); j++) {
    Expression * termJ = const_cast<Expression *>(a->operand(j));
    replaceOperand(operand(i), termJ->clone(), false);
    Expression * m = clone();
    a->replaceOperand(termJ, m, true);
    m->immediateSimplify();
  }
  replaceWith(a, true);
  return a->immediateSimplify();
}

const Expression * Multiplication::CreateExponent(Expression * e) {
  Expression * n = e->type() == Type::Power ? e->operand(1)->clone() : new Rational(Integer(1));
  return n;
}

bool Multiplication::TermsHaveIdenticalBase(const Expression * e1, const Expression * e2) {
  const Expression * f1 = e1->type() == Type::Power ? e1->operand(0) : e1;
  const Expression * f2 = e2->type() == Type::Power ? e2->operand(0) : e2;
  return (f1->compareTo(f2) == 0);
}

bool Multiplication::TermsHaveIdenticalNonUnitaryExponent(const Expression * e1, const Expression * e2) {
  Rational one = Rational(1);
  return e1->type() == Type::Power && e2->type() == Type::Power && (e1->operand(1)->compareTo(e2->operand(1)) == 0);
}

bool Multiplication::TermHasRationalBase(const Expression * e) {
  bool hasRationalBase = e->type() == Type::Power ? e->operand(0)->type() == Type::Rational : e->type() == Type::Rational;
  return hasRationalBase;
}

bool Multiplication::TermHasRationalExponent(const Expression * e) {
  bool hasRationalExponent = e->type() == Type::Power ? e->operand(1)->type() == Type::Rational : true;
  return hasRationalExponent;
}

bool Multiplication::isUselessOperand(const Rational * r) {
  return r->isOne();
}

Expression * Multiplication::immediateBeautify() {
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  Expression * e = mergeNegativePower();
  if (e->type() == Type::Power) {
    return e->immediateBeautify();
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
      Power * p = static_cast<Power *>((Expression *)operand(index));
      Expression * denominatorOperand = const_cast<Expression *>(p->operand(0));
      p->detachOperand(denominatorOperand);
      removeOperand(p, true);
      Multiplication * numeratorOperand = (Multiplication *)clone();
      const Expression * divOperands[2] = {numeratorOperand, denominatorOperand};
      Division * d = new Division(divOperands, false);
      /* We want 1/3*Pi*(ln(2))^-1 -> Pi/(3ln(2)) and not ((1/3)Pi)/ln(2)*/
      if (numeratorOperand->operand(0)->type() == Type::Rational && !static_cast<const Rational *>(numeratorOperand->operand(0))->denominator().isOne()) {
        Rational * r = static_cast<Rational *>((Expression *)numeratorOperand->operand(0));
        if (denominatorOperand->type() == Type::Multiplication) {
          const Expression * integerDenominator[1] = {new Rational(r->denominator())};
          static_cast<Multiplication *>(denominatorOperand)->addOperands(integerDenominator, 1);
          static_cast<Multiplication *>(denominatorOperand)->sortChildren();
        } else {
          const Expression * multOperands[2] = {new Rational(r->denominator()), denominatorOperand->clone()};
          Multiplication * m = new Multiplication(multOperands, 2, false);
          denominatorOperand->replaceWith(m, true);
        }
        if ((!r->numerator().isMinusOne() && !r->numerator().isOne()) || numeratorOperand->numberOfOperands() == 1) {
          numeratorOperand->replaceOperand(r, new Rational(r->numerator()), true);
        } else {
          if (r->numerator().isOne()) {
            numeratorOperand->removeOperand(r, true);
          } else {
            const Expression * oppOperand[1] = {numeratorOperand->clone()};
            Opposite * o = new Opposite(oppOperand, false);
            numeratorOperand->replaceWith(o, true);
          }
        }
      }
      Expression * newNumeratorOperand = numeratorOperand->squashUnaryHierarchy();
      // Delete parenthesis unnecessary on numerator
      if (newNumeratorOperand->type() == Type::Parenthesis) {
        newNumeratorOperand->replaceWith((Expression *)newNumeratorOperand->operand(0), true);
      }
      replaceWith(d, true);
      return d->immediateBeautify();
    }
  }
  // -1*A -> -A
  if (operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(0))->isMinusOne()) {
    removeOperand((Expression *)operand(0), true);
    Expression * e = squashUnaryHierarchy();
    const Expression * oppOperand[1] = {e->clone()};
    Opposite * o = new Opposite(oppOperand, false);
    e->replaceWith(o, true);
    return o;
  }
  return this;
}

Expression * Multiplication::createDenominator() {
  // Merge negative power: a*b^-1*c^(-Pi)*d = a*(b*c^Pi)^-1
  Expression * e = mergeNegativePower();
  if (e->type() == Type::Power) {
    return static_cast<Power *>(e)->createDenominator();
  }
  assert(e == this);
  for (int index = 0; index < numberOfOperands(); index++) {
    // a*b^(-1)*... -> a*.../b
    if (operand(index)->type() == Type::Power && operand(index)->operand(1)->type() == Type::Rational && static_cast<const Rational *>(operand(index)->operand(1))->isMinusOne()) {
      Power * p = static_cast<Power *>((Expression *)operand(index));
      return p->operand(0)->clone();
    }
  }
  return nullptr;
}

Expression * Multiplication::mergeNegativePower() {
  Multiplication * m = new Multiplication();
  int i = 0;
  while (i < numberOfOperands()) {
    if (operand(i)->type() == Type::Power && operand(i)->operand(1)->sign() < 0) {
      const Expression * e = operand(i);
      const_cast<Expression *>(e->operand(1))->turnIntoPositive();
      removeOperand(e, false);
      m->addOperands(&e, 1);
      const_cast<Expression *>(e)->immediateSimplify();
    } else {
      i++;
    }
  }
  if (m->numberOfOperands() == 0) {
    return this;
  }
  const Expression * powOperands[2] = {m, new Rational(Integer(-1))};
  Power * p = new Power(powOperands, false);
  m->sortChildren();
  m->squashUnaryHierarchy();
  const Expression * multOperand[1] = {p};
  addOperands(multOperand, 1);
  sortChildren();
  return squashUnaryHierarchy();
}

void Multiplication::leastCommonMultiple(Expression * factor) {
  if (factor->type() == Type::Multiplication) {
    for (int j = 0; j < factor->numberOfOperands(); j++) {
      leastCommonMultiple((Expression *)factor->operand(j));
    }
    return;
  }
  for (int i = 0; i < numberOfOperands(); i++) {
    if (TermsHaveIdenticalBase(operand(i), factor)) {
      const Expression * index[2] = {CreateExponent((Expression *)operand(i)), CreateExponent(factor)};
      Subtraction * sub = new Subtraction(index, false);
      Expression::simplifyAndBeautify((Expression **)&sub);
      if (sub->sign() < 0) { // index[0] < index[1]
        factor->replaceOperand((Expression *)factor->operand(1), new Opposite((Expression **)&sub, true), true);
        factor->simplify();
        factorizeBase((Expression *)operand(i), factor);
      } else if (sub->sign() == 0) {
        factorizeBase((Expression *)operand(i), factor);
      } else {}
      delete sub;
      return;
    }
  }
  const Expression * newOp[1] = {factor->clone()};
  addOperands(newOp, 1);
  sortChildren();
}

template Poincare::Evaluation<float>* Poincare::Multiplication::computeOnComplexAndMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Multiplication::computeOnComplexAndMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*);
}
template Poincare::Complex<float> Poincare::Multiplication::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Poincare::Complex<double> Poincare::Multiplication::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);
