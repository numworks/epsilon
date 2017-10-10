#include <poincare/addition.h>
#include <poincare/complex_matrix.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/opposite.h>
#include <poincare/undefined.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

Expression::Type Addition::type() const {
  return Type::Addition;
}

Expression * Addition::clone() const {
  return new Addition(operands(), numberOfOperands(), true);
}

template<typename T>
Complex<T> Addition::compute(const Complex<T> c, const Complex<T> d) {
  return Complex<T>::Cartesian(c.a()+d.a(), c.b()+d.b());
}

void Addition::immediateSimplify() {
  /* TODO: optimize, do we have to restart index = 0 at every merging? */
  int index = 0;
  while (index < numberOfOperands()) {
    Expression * o = (Expression *)operand(index++);
    if (o->type() == Type::Addition) {
      mergeOperands(static_cast<Addition *>(o));
      index = 0;
    } else if (o->type() == Type::Undefined) {
      replaceWith(new Undefined(), true);
      return;
    }
  }
  sortChildren();
  int i = 0;
  while (i < numberOfOperands()-1) {
    if (operand(i)->type() == Type::Rational && operand(i+1)->type() == Type::Rational) {
      Rational a = Rational::Addition(*(static_cast<const Rational *>(operand(i))), *(static_cast<const Rational *>(operand(i+1))));
      replaceOperand(operand(i), new Rational(a), true);
      removeOperand(operand(i+1), true);
    } else if (TermsHaveIdenticalNonRationalFactors(operand(i), operand(i+1))) {
      factorizeChildren(const_cast<Expression *>(operand(i)), const_cast<Expression *>(operand(i+1)));
      if (numberOfOperands() > 1 && operand(i)->type() == Type::Rational && static_cast<const Rational *>(operand(i))->isZero()) {
        removeOperand(operand(i), true);
        if (i > 0) {
          i--;
        }
      }
    } else {
      i++;
    }
  }
  if (numberOfOperands() > 1 && operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(0))->isZero()) {
    removeOperand(operand(0), true);
  }
  if (numberOfOperands() == 1) {
    replaceWith(const_cast<Expression *>(operand(0)), true);
  }
}

void Addition::factorizeChildren(Expression * e1, Expression * e2) {
  Rational * r = new Rational(Rational::Addition(RationalFactor(e1), RationalFactor(e2)));
  removeOperand(e2, true);
  if (e1->type() == Type::Multiplication) {
    if (e1->operand(0)->type() == Type::Rational) {
      e1->replaceOperand(e1->operand(0), r, true);
    } else {
      static_cast<Multiplication *>(e1)->addOperandAtIndex(r, 0);
    }
    e1->immediateSimplify();
  } else {
    const Expression * operands[2] = {r, e1};
    Multiplication * m = new Multiplication(operands, 2, true);
    e1->replaceWith(m, true);
    m->immediateSimplify();
  }
}

const Rational Addition::RationalFactor(Expression * e) {
  if (e->type() == Type::Multiplication && e->operand(0)->type() == Type::Rational) { // TODO: change integer for Rational
    return *(static_cast<const Rational *>(e->operand(0)));
  }
  return Rational(Integer(1));
}


bool Addition::TermsHaveIdenticalNonRationalFactors(const Expression * e1, const Expression * e2) {
  if (e1->type() == Type::Multiplication && e2->type() == Type::Multiplication) {
    return Multiplication::HaveSameNonRationalFactors(e1, e2);
  }
  const Expression * f1 = (e1->type() == Type::Multiplication && e1->numberOfOperands() == 2 && e1->operand(0)->type() == Type::Rational) ? e1->operand(1) : e1;
  const Expression * f2 = (e2->type() == Type::Multiplication && e2->numberOfOperands() == 2 && e2->operand(0)->type() == Type::Rational) ? e2->operand(1) : e2;
  return (f1->compareTo(f2) == 0);
}

void Addition::immediateBeautify() {
  int index = 0;
  while (index < numberOfOperands()) {
    // a+(-1)*b+... -> a-b+...
    if (operand(index)->type() == Type::Multiplication && static_cast<const Multiplication *>(operand(index))->operand(0)->type() == Type::Rational && static_cast<const Rational *>(operand(index)->operand(0))->isMinusOne()) {
      Multiplication * m = static_cast<Multiplication *>((Expression *)operand(index));
      m->removeOperand(m->operand(0), true);
      if (m->numberOfOperands() == 1) {
        m->replaceWith(const_cast<Expression *>(m->operand(0)), true);
      }
      const Expression * replacedOperand = operand(index);
      if (index == 0) {
        const Expression * opOperand[1] = {replacedOperand};
        Opposite * o = new Opposite(opOperand, false);
        replaceOperand(const_cast<Expression *>(replacedOperand), o, false);
      } else {
        const Expression * subOperands[2] = {operand(index-1), replacedOperand};
        removeOperand(operand(index-1), false);
        Subtraction * s = new Subtraction(subOperands, false);
        replaceOperand(const_cast<Expression *>(replacedOperand), s, false);
      }
    }
    index++;
  }
}

template Poincare::Complex<float> Poincare::Addition::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Poincare::Complex<double> Poincare::Addition::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnMatrices<float>(Poincare::Evaluation<float>*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnMatrices<double>(Poincare::Evaluation<double>*, Poincare::Evaluation<double>*);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnComplexAndMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnComplexAndMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*);

}
