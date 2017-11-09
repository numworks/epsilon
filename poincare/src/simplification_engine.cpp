#include <poincare/simplification_engine.h>

namespace Poincare {

Expression * SimplificationEngine::map(Expression * e, Context & context, Expression::AngleUnit angleUnit) {
  assert(e->numberOfOperands() == 1 && e->operand(0)->type() == Expression::Type::Matrix);
  Expression * op = e->editableOperand(0);
  for (int i = 0; i < op->numberOfOperands(); i++) {
    Expression * entry = op->editableOperand(i);
    e->replaceOperand(op, entry, false);
    op->replaceOperand(entry, op, false);
  }
  return e->replaceWith(op, true)->shallowReduce(context, angleUnit);
}

}
