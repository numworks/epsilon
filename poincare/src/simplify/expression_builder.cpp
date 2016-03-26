#include "expression_builder.h"
#include <poincare/addition.h>
#include <poincare/integer.h>
extern "C" {
#include <assert.h>
}

/* Our compiler defines two things:
 * - First, a list of ExpressionBuilder
 * - Second, a children_expression_buffer[] of the proper size.
 */

Expression * children_expression_buffer[4]; // This will be defined by our compiler

Expression * ExpressionBuilder::build(Expression * matches[]) {
  Expression * children_expressions[255]; // FIXME: <- The sized can be given by the compiler
  //Expression * children_expressions = malloc;
// That malloc can be avoided: we can give an upper bound
// Afer we've finished processing the rules
// That upper bound is likely to be very small (3, currently)

  for (int i=0; i<m_numberOfChildren; i++) {
    ExpressionBuilder * child = this->child(i);
    children_expressions[i] = child->build(matches);
  }
  Expression * result = nullptr;
  switch(m_action) {
    case ExpressionBuilder::Action::Build:
      switch(m_expressionType) {
        case Expression::Type::Addition:
          result = new Addition(children_expressions, m_numberOfChildren, true);
          break;
        case Expression::Type::Integer:
          result = new Integer(m_integerValue);
          break;
        default:
          assert(false);
          break;
      }
      break;
    case ExpressionBuilder::Action::Clone:
      result = matches[m_matchIndex]->clone();
      break;
    case ExpressionBuilder::Action::FunctionCall:
      // result = m_functionPointer(children_expressions);
      break;
  }
  return result;
}

// Extrude in a class
ExpressionBuilder * ExpressionBuilder::child(int index) {
  assert(index>=0 && index<m_numberOfChildren);
  if (index == 0) {
    return (this+1); // Pointer arithmetics
  } else {
    ExpressionBuilder * previousChild = this->child(index-1);
    return previousChild+previousChild->m_numberOfChildren+1; // Pointer arithm.
  }
}
