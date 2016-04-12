#include "expression_builder.h"
#include <poincare/addition.h>
#include <poincare/integer.h>
#include <poincare/product.h>
#include <poincare/symbol.h>
extern "C" {
#include <assert.h>
}

Expression * ExpressionBuilder::build(ExpressionMatch matches[]) {
  Expression * children_expressions[255]; // FIXME: <- The sized can be given by the compiler
  //Expression * children_expressions = malloc;
  // That malloc can be avoided: we can give an upper bound
  // Afer we've finished processing the rules
  // That upper bound is likely to be very small (3, currently)
  int numberOfChildrenExpressions = 0;

  for (int i=0; i<m_numberOfChildren; i++) {
    ExpressionBuilder * child = this->child(i);
    if (child->m_action == ExpressionBuilder::Action::BringUpWildcard) {
      for (int j=0; j<matches[child->m_matchIndex].numberOfExpressions(); j++) {
        children_expressions[numberOfChildrenExpressions++] =
          matches[child->m_matchIndex].expression(j)->clone();
      }
    } else {
      children_expressions[numberOfChildrenExpressions++] = child->build(matches);
    }
  }

  Expression * result = nullptr;
  switch(m_action) {
    case ExpressionBuilder::Action::BuildFromType:
      switch(m_expressionType) {
        case Expression::Type::Addition:
          /* The children do not need to be cloned as they already have been
           * before. */
          result = new Addition(children_expressions, numberOfChildrenExpressions, false);
          break;
        case Expression::Type::Product:
          /* The children do not need to be cloned as they already have been
           * before. */
          result = new Product(children_expressions, numberOfChildrenExpressions, false);
          break;
        default:
          assert(false);
          break;
      }
      break;
    case ExpressionBuilder::Action::BuildFromTypeAndValue:
      switch(m_expressionType) {
        case Expression::Type::Integer:
          result = new Integer(m_integerValue);
          break;
        case Expression::Type::Symbol:
          result = new Symbol(m_symbolName);
          break;
        default:
          assert(false);
          break;
      }
      break;
    case ExpressionBuilder::Action::BringUpWildcard:
      // Build should never be called on BringUpWildcard action directly
      assert(false);
      break;
    case ExpressionBuilder::Action::Clone:
      // It only makes sense to clone if the match has a single expression!
      assert(matches[m_matchIndex].numberOfExpressions() == 1);
      result = matches[m_matchIndex].expression(0)->clone();
      break;
    case ExpressionBuilder::Action::CallExternalGenerator:
      result = m_generator(children_expressions, numberOfChildrenExpressions);
      break;
  }
  return result;
}

ExpressionBuilder * ExpressionBuilder::child(int index) {
  assert(index>=0 && index<m_numberOfChildren);
  if (index == 0) {
    return (this+1); // Pointer arithmetics
  } else {
    ExpressionBuilder * previousChild = this->child(index-1);
    return previousChild+previousChild->m_numberOfChildren+1; // Pointer arithm.
  }
}
