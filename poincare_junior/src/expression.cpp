#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/expressions.h>
#include <poincare_junior/src/memory/cache_pool.h>

namespace Poincare {

// TODO dummy parse
Expression Expression::Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return Expression([](const char * text){
      Node::Push<Division>();
      Node::Push<Division>();
      Node::Push<Subtraction>();
      Node::Push<IntegerShort>(1);
      Node::Push<IntegerShort>(2);
      Node::Push<IntegerShort>(3);
      Node::Push<IntegerShort>(4);
    }, textInput);
}

Expression Expression::CreateBasicReduction(void * expressionAddress) {
  return Expression(
    [](Node tree) {
      tree.recursivelyEdit([](Node node) {
          EExpression::BasicReduction(node.block());
        });
    },
    expressionAddress);
}

float Expression::approximate(float x) const {
  float res;
  send(
    [](const Node tree, void * res) {
      float * result = static_cast<float *>(res);
      *result = EExpression::Approximate(tree.block());
    },
    &res
  );
  return res;
}

}
