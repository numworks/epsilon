#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/arithmetic.h>
#include <poincare/integer.h>

namespace Poincare {

/* Those transforms compute the sum of rational numbers
 *
 * The strategy is rather simple:
 *  - Match an expression of the shape a/b + c/d (variables being Integers)
 *  - Replace it with (a*d+c*b)/(d*c)
 *
 * In practice, the initial expression nearly always already contains a division
 * so we simply reuse it. */

static bool PrivateRationalAdditionTransform(Integer * n1, Integer * d1, Integer * n2, Integer * d2, Expression * operandToRemove) {
  // n1 and d1 will be replaced, so they must NOT be a descendant of operandToDelete
  assert(!n1->hasAncestor(operandToRemove));
  assert(!d1->hasAncestor(operandToRemove));
  assert(operandToRemove->parent()->type() == Expression::Type::Addition);

  Integer * newNumerator = new Integer(Integer::Addition(Integer::Multiplication(*n1, *d2), Integer::Multiplication(*n2, *d1)));
  Integer * newDenominator = new Integer(Integer::Multiplication(*d1, *d2));

  n1->replaceWith(newNumerator);
  d1->replaceWith(newDenominator);
  operandToRemove->removeFromParent();

  return true;
}

bool Simplification::RationalAdditionTransform(Expression * captures[]) {
  // If we match a/b+c/d, we modify a and b and remove c/d.
  Integer * n1 = static_cast<Integer *>(captures[0]);
  Integer * d1 = static_cast<Integer *>(captures[1]);
  Integer * n2 = static_cast<Integer *>(captures[2]);
  Integer * d2 = static_cast<Integer *>(captures[3]);
  Multiplication * m2 = static_cast<Multiplication *>(n2->parent());
  assert(n1->type() == Expression::Type::Integer);
  assert(d1->type() == Expression::Type::Integer);
  assert(n2->type() == Expression::Type::Integer);
  assert(d2->type() == Expression::Type::Integer);
  assert(m2->type() == Expression::Type::Multiplication);
  return PrivateRationalAdditionTransform(n1, d1, n2, d2, m2);
}

bool Simplification::IntegerRationalAdditionTransform(Expression * captures[]) {
  // We matched a/b + c.
  // We modify a and b, and remove c.
  Integer * n1 = static_cast<Integer *>(captures[0]);
  Integer * d1 = static_cast<Integer *>(captures[1]);
  Integer * n2 = static_cast<Integer *>(captures[2]);
  assert(n1->type() == Expression::Type::Integer);
  assert(d1->type() == Expression::Type::Integer);
  assert(n2->type() == Expression::Type::Integer);
  Integer d2(1);
  return PrivateRationalAdditionTransform(n1, d1, n2, &d2, n2);
}

bool Simplification::InverseIntegerRationalAdditionTransform(Expression * captures[]) {
  // We matched a/b + 1/c.
  // We modify a and b, and remove 1/c.
  Integer * n1 = static_cast<Integer *>(captures[0]);
  Integer * d1 = static_cast<Integer *>(captures[1]);
  Integer * d2 = static_cast<Integer *>(captures[2]);
  assert(n1->type() == Expression::Type::Integer);
  assert(d1->type() == Expression::Type::Integer);
  assert(d2->type() == Expression::Type::Integer);
  Integer n2 = Integer(1);
  Expression * operandToRemove = d2->parent();
  assert(operandToRemove->type() == Expression::Type::Power);

  return PrivateRationalAdditionTransform(n1, d1, &n2, d2, operandToRemove);
}

bool Simplification::InverseIntegerAdditionTransform(Expression * captures[]) {
  // We matched a^-1 + b^-1
  // This case is a tad bit more annoying because there's no numerator.
  // Let's just add one!
  Integer * n1 = new Integer(1);
  Integer * d1 = static_cast<Integer *>(captures[0]);
  Integer n2(1);
  Integer * d2 = static_cast<Integer *>(captures[1]);

  assert(d1->type() == Expression::Type::Integer);
  assert(d2->type() == Expression::Type::Integer);

  Expression * p = d1->parent();
  assert(p->type() == Expression::Type::Power);
  Expression * root = p->parent();
  assert(root != nullptr);

  Expression * multOperands[2] = {n1, p};
  Multiplication * m = new Multiplication(multOperands, 2, false); // Don't clone anyone
  root->replaceOperand(p, m, false); // But don't delete p either!

  return PrivateRationalAdditionTransform(n1, d1, &n2, d2, d2->parent());
}

}
