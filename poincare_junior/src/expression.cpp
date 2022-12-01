#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace Poincare {

// TODO dummy parse
Expression Expression::Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return Expression([](const char * text){
      Node::Push<BlockType::Division>();
      Node::Push<BlockType::Division>();
      Node::Push<BlockType::Subtraction>();
      Node::Push<BlockType::IntegerShort>(1);
      Node::Push<BlockType::IntegerShort>(2);
      Node::Push<BlockType::IntegerShort>(3);
      Node::Push<BlockType::IntegerShort>(4);
    }, textInput);
}

Expression Expression::CreateBasicReduction(void * expressionAddress) {
  return Expression(
    [](Node tree) {
      EditionReference(tree).recursivelyEdit([](EditionReference reference) {
          Simplification::BasicReduction(reference);
        });
    },
    expressionAddress);
}

float Expression::approximate(float x) const {
  float res;
  send(
    [](const Node tree, void * res) {
      float * result = static_cast<float *>(res);
      *result = Approximation::To<float>(tree);
    },
    &res
  );
  return res;
}

}
