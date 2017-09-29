#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/arithmetic.h>
#include <poincare/integer.h>

bool Poincare::Simplification::RationalAdditionTransform(Expression * captures[]) {
  Integer * n1 = (Integer *)(captures[2]);
  Integer * d1 = (Integer *)(captures[4]);
  Integer * n2 = (Integer *)(captures[6]);
  Integer * d2 = (Integer *)(captures[8]);
  Integer * n = new Integer(Integer::Addition(Integer::Multiplication(*n1,*d2), Integer::Multiplication(*n2,*d1)));
  Integer * d = new Integer(Integer::Multiplication(*d1, *d2));

  Multiplication * m1 = static_cast<Multiplication *>(captures[1]);
  m1->replaceOperand(n1, n, true);
  Power * p1 = static_cast<Power *>(captures[3]);
  p1->replaceOperand(d1, d, true);

  Addition * a = static_cast<Addition *>(captures[0]);

  if (a->numberOfOperands() == 2) {
    a->replaceOperand(m1, nullptr, false);
    static_cast<Hierarchy *>(a->parent())->replaceOperand(a, m1, true);
  } else {
    Multiplication * m2 = static_cast<Multiplication *>(captures[5]);
    assert(a->numberOfOperands() > 2);
    a->removeOperand(m2);
  }
  return true;
}
