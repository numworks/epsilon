#include "expression.h"
#include "cache_pool.h"

namespace Poincare {

Expression Expression::CreateBasicReduction(void * expressionAddress) {
  return Expression(
    [](Node tree) {
      tree.expressionInterface()->basicReduction(tree.block());
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
