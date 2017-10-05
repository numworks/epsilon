#include <poincare/addition.h>
#include <poincare/complex_matrix.h>
#include <poincare/multiplication.h>
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

void Addition::privateSimplify() {
  for (int i=0; i<numberOfOperands(); i++) {
    Expression * o = (Expression *)operand(i);
    if (o->type() == Type::Addition) {
      mergeOperands(static_cast<Addition *>(o));
    }
  }
  sortChildren();
  for (int i = 0; i < numberOfOperands()-1; i++) {
    // TODO: maybe delete operand Integer(0) and at the end test if the numberOfOperand is 0, return Integer(0)?
    if (operand(i)->type() == Type::Integer && operand(i+1)->type() == Type::Integer) {
      Integer a = Integer::Addition(*(static_cast<const Integer *>(operand(i))), *(static_cast<const Integer *>(operand(i+1))));
      replaceOperand(operand(i), new Integer(a), true);
      removeOperand(operand(i+1), true);
    } else if (TermsHaveIdenticalNonRationalFactors(operand(i), operand(i+1))) {
      factorizeChildren(const_cast<Expression *>(operand(i)), const_cast<Expression *>(operand(i+1)));
    }
  }
  if (numberOfOperands() == 1) {
    replaceWith(const_cast<Expression *>(operand(0)), true);
  }
}

void Addition::factorizeChildren(Expression * e1, Expression * e2) {
  Integer * r = new Integer(Integer::Addition(RationalFactor(e1), RationalFactor(e2)));
  removeOperand(e2, true);
  if (e1->type() == Type::Multiplication) {
    if (e1->operand(0)->type() == Type::Integer) {
      e1->replaceOperand(e1->operand(0), r, true);
    } else {
      static_cast<Multiplication *>(e1)->addOperandAtIndex(r, 0);
    }
  } else {
    const Expression * operands[2] = {r, e1};
    e1->replaceWith(new Multiplication(operands, 2, true), true);
    sortChildren();
  }
}

const Integer Addition::RationalFactor(Expression * e) {
  if (e->type() == Type::Multiplication && e->operand(0)->type() == Type::Integer) { // TODO: change integer for Rational
    return *(static_cast<const Integer *>(e->operand(0)));
  }
  return Integer(1);
}


bool Addition::TermsHaveIdenticalNonRationalFactors(const Expression * e1, const Expression * e2) {
  if (e1->type() == Type::Multiplication && e2->type() == Type::Multiplication) {
    return Multiplication::HaveSameNonRationalFactors(e1, e2);
  }
  const Expression * f1 = (e1->type() == Type::Multiplication && e1->numberOfOperands() == 2 && e1->operand(0)->type() == Type::Integer) ? e1->operand(1) : e1;
  const Expression * f2 = (e2->type() == Type::Multiplication && e2->numberOfOperands() == 2 && e2->operand(0)->type() == Type::Integer) ? e2->operand(1) : e2;
  return (f1->compareTo(f2) == 0);
}

template Poincare::Complex<float> Poincare::Addition::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Poincare::Complex<double> Poincare::Addition::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnMatrices<float>(Poincare::Evaluation<float>*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnMatrices<double>(Poincare::Evaluation<double>*, Poincare::Evaluation<double>*);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnComplexAndMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnComplexAndMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*);

}
