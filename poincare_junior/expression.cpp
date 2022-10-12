#include "expression.h"
#include "cache_pool.h"

namespace Poincare {

// TODO dummy parse
Expression Expression::Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return Expression([](const char * text){
      DivisionInterface::PushNode();
      DivisionInterface::PushNode();
      SubtractionInterface::PushNode();
      IntegerInterface::PushNode(1);
      IntegerInterface::PushNode(2);
      IntegerInterface::PushNode(3);
      IntegerInterface::PushNode(4);
    }, textInput);
}

Expression Expression::CreateBasicReduction(void * expressionAddress) {
  return Expression(
    [](Node tree) {
      tree.recursivelyEdit([](Node node) {
          node.expressionInterface()->basicReduction(node.block());
        });
    },
    expressionAddress);
}

float Expression::approximate(float x) const {
  float res;
  send(
    [](const Node tree, void * res) {
      float * result = static_cast<float *>(res);
      *result = tree.expressionInterface()->approximate(tree.block());
    },
    &res
  );
  return res;
}

}
