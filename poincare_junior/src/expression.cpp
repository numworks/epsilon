#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

EditionReference Expression::ParseFromLayoutInEditionPool(Node node) {
  // node == (1-2)/3/4
  EditionReference ref = EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::Division>();
  EditionReference::Push<BlockType::Subtraction>();
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(1));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(2));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(3));
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(4));
  // Remove node from EditionReference
  EditionReference nodeRef(node);
  nodeRef.removeTree();
  return ref;
}

Expression Expression::ParseFromText(const char * textInput) {
  return Expression([](const char * text){
      ParseFromLayoutInEditionPool(Layout::ParseFromTextInEditionPool(text));
    }, textInput);
}

Expression Expression::Parse(const Layout * layoutInput) {
  return Expression([](Node node){
      ParseFromLayoutInEditionPool(node);
    }, layoutInput);
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
