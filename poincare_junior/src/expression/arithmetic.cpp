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

}  // namespace PoincareJ
