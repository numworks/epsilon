#include <poincare/simplification_engine.h>

#if MATRIX_EXACT_REDUCING

namespace Poincare {

Expression * SimplificationEngine::map(Expression * e, Context & context, Expression::AngleUnit angleUnit) {
  assert(e->numberOfOperands() == 1 && e->operand(0)->type() == Expression::Type::Matrix);
  Expression * op = e->editableOperand(0);
  for (int i = 0; i < op->numberOfOperands(); i++) {
    Expression * entry = op->editableOperand(i);
    Expression * eCopy = e->clone();
    eCopy->replaceOperand(eCopy->editableOperand(0), entry, true);
    op->replaceOperand(entry, eCopy, false);
    eCopy->shallowReduce(context, angleUnit);
  }
  return e->replaceWith(op, true)->shallowReduce(context, angleUnit);
}

}

#endif
