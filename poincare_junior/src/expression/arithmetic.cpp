#include "arithmetic.h"

#include <poincare_junior/src/memory/exception_checkpoint.h>

#include "rational.h"

namespace PoincareJ {

bool Arithmetic::SimplifyQuotientOrRemainder(Tree* expr) {
  assert(expr->numberOfChildren() == 2);
  bool isQuotient = expr->type() == BlockType::Quotient;
  const Tree* num = expr->firstChild();
  const Tree* denom = num->nextTree();
  if (!num->type().isInteger() || !denom->type().isInteger()) {
    if (num->type().isRational() || denom->type().isRational()) {
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
    }
    return false;
  }
  if (denom->type() == BlockType::Zero) {
    ExceptionCheckpoint::Raise(ExceptionType::ZeroDivision);
  }
  IntegerHandler n = Integer::Handler(num);
  IntegerHandler d = Integer::Handler(denom);
  expr->moveTreeOverTree(isQuotient ? IntegerHandler::Quotient(n, d)
                                    : IntegerHandler::Remainder(n, d));
  return true;
}

bool Arithmetic::SimplifyGCD(Tree* expr) {
  const Tree* a = expr->firstChild();
  const Tree* b = a->nextTree();
  if (!a->type().isInteger() || !b->type().isInteger()) {
    if (a->type().isRational() || b->type().isRational()) {
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
    }
    return false;
  }
  expr->moveTreeOverTree(
      IntegerHandler::GCD(Integer::Handler(a), Integer::Handler(b)));
  return true;
}

}  // namespace PoincareJ
