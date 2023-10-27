#include "arithmetic.h"

#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"
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
  bool changed = NAry::Flatten(expr);
  // TODO test type on the fly to reduce gcd(2,4,x) into gcd(2,x)
  for (const Tree* child : expr->children()) {
    if (!child->type().isInteger()) {
      if (child->type().isRational()) {
        ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
      }
      return changed;
    }
  }
  Tree* first = expr->firstChild();
  Tree* next = first->nextTree();
  int n = expr->numberOfChildren();
  while (n-- > 1) {
    // TODO keep a handler on first out of the loop
    first->moveTreeOverTree(
        IntegerHandler::GCD(Integer::Handler(first), Integer::Handler(next)));
    next = first->nextTree();
    next->removeTree();
  }
  expr->removeNode();
  return true;
}

bool Arithmetic::SimplifyLCM(Tree* expr) {
  bool changed = NAry::Flatten(expr);
  // TODO test type on the fly
  for (const Tree* child : expr->children()) {
    if (!child->type().isInteger()) {
      if (child->type().isRational()) {
        ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
      }
      return changed;
    }
  }
  Tree* first = expr->firstChild();
  Tree* next = first->nextTree();
  int n = expr->numberOfChildren();
  while (n-- > 1) {
    first->moveTreeOverTree(
        IntegerHandler::LCM(Integer::Handler(first), Integer::Handler(next)));
    next = first->nextTree();
    next->removeTree();
  }
  expr->removeNode();
  return true;
}

}  // namespace PoincareJ
