#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

// TODO dummy parse
Expression Expression::Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return Expression([](const char * text){
      EditionReference::Push<BlockType::Division>();
      EditionReference::Push<BlockType::Division>();
      EditionReference::Push<BlockType::Subtraction>();
      EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
      EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
      EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
      EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(4));
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
