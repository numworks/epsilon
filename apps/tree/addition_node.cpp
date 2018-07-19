#include "addition_node.h"
#include "float_node.h"

bool AdditionNode::shallowReduce() {
  if (ExpressionNode::shallowReduce()) {
    return true;
  }
  /* Step 1: Addition is associative, so let's start by merging children which
   * also are additions themselves. */
  int i = 0;
  int initialNumberOfChildren = numberOfChildren();
  while (i < initialNumberOfChildren) {
    ExpressionNode * currentChild = child(i);
    if (currentChild->type() == Type::Addition) {
      TreeRef(this).mergeChildren(TreeRef(currentChild));
      // Is it ok to modify memory while executing ?
      continue;
    }
    i++;
  }

  // Step 2: Sort the operands
  sortChildren();

  /* Step 3: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  i = 0;
  while (i < numberOfChildren()-1) {
    ExpressionNode * e1 = child(i);
    ExpressionNode * e2 = child(i+1);
    if (e1->type() == Type::Float && e2->type() == Type::Float) {
      float sum = e1->approximate() + e2->approximate();
      // Remove first e2 then e1, else the pointers change
      removeChild(e2);
      removeChild(e1);
      FloatRef f(sum);
      addChildAtIndex(f.node(), i);
      continue;
    }
    /*if (TermsHaveIdenticalNonRationalFactors(e1, e2)) { //TODO
      factorizeOperands(e1, e2); //TODO
      continue;
      }*/
    i++;
  }

  return false;
}
