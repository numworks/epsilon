#include "expressions/expression.h"
#include "expressions/expressions.h"
#include "expression_reference.h"
#include "cache_pool.h"

namespace Poincare {

// TODO dummy parse
ExpressionReference ExpressionReference::Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return ExpressionReference([](const char * text){
      Node::Push<Division>();
      Node::Push<Division>();
      Node::Push<Subtraction>();
      Node::Push<IntegerShort>(1);
      Node::Push<IntegerShort>(2);
      Node::Push<IntegerShort>(3);
      Node::Push<IntegerShort>(4);
    }, textInput);
}

ExpressionReference ExpressionReference::CreateBasicReduction(void * expressionAddress) {
  return ExpressionReference(
    [](Node tree) {
      tree.recursivelyEdit([](Node node) {
          Expression::BasicReduction(node.block());
        });
    },
    expressionAddress);
}

float ExpressionReference::approximate(float x) const {
  float res;
  send(
    [](const Node tree, void * res) {
      float * result = static_cast<float *>(res);
      *result = Expression::Approximate(tree.block());
    },
    &res
  );
  return res;
}

}
