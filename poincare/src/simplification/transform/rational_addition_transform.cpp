#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/arithmetic.h>
#include <poincare/integer.h>

namespace Poincare {

static bool PrivateRationalAdditionTransform(Expression * captures[], const Integer n2, const Integer d2) {
  // captures[0->4] is OK
  // Addition.a(Multiplication!.b(Integer.c,Power.d(Integer.e, Integer[-1])), captures[5]))
  Integer * n1 = (Integer *)(captures[2]);
  Integer * d1 = (Integer *)(captures[4]);
  Integer * n = new Integer(Integer::Addition(Integer::Multiplication(*n1,d2), Integer::Multiplication(n2,*d1)));
  Integer * d = new Integer(Integer::Multiplication(*d1, d2));

  Multiplication * m1 = static_cast<Multiplication *>(captures[1]);
  m1->replaceOperand(n1, n, true);
  Power * p1 = static_cast<Power *>(captures[3]);
  p1->replaceOperand(d1, d, true);

  Addition * a = static_cast<Addition *>(captures[0]);

  if (a->numberOfOperands() == 2) {
    a->replaceOperand(m1, nullptr, false);
    static_cast<Hierarchy *>(a->parent())->replaceOperand(a, m1, true);
  } else {
    assert(a->numberOfOperands() > 2);
    a->removeOperand(captures[5]);
  }
  return true;
}

bool Simplification::RationalAdditionTransform(Expression * captures[]) {
  Integer * n2 = (Integer *)(captures[6]);
  Integer * d2 = (Integer *)(captures[8]);
  return PrivateRationalAdditionTransform(captures, *n2, *d2);
}

bool Simplification::IntegerRationalAdditionTransform(Expression * captures[]) {
  Integer * n2 = (Integer *)(captures[5]);
  return PrivateRationalAdditionTransform(captures, *n2, Integer(1));
}

bool Simplification::InverseIntegerRationalAdditionTransform(Expression * captures[]) {
  Integer * d2 = (Integer *)(captures[6]);
  return PrivateRationalAdditionTransform(captures, Integer(1), *d2);
}

}
