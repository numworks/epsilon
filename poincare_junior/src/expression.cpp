#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

EditionReference Expression::EditionPoolExpressionToLayout(Node node) {
  assert(node.block()->isExpression());
  // node == -1+2*3
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(6);
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('-');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('1');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('+');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('2');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('*');
  EditionReference::Push<BlockType::CodePointLayout, CodePoint>('3');
  // Remove node from EditionReference
  EditionReference nodeRef(node);
  nodeRef.removeTree();
  return ref;
}

EditionReference Expression::EditionPoolLayoutToExpression(Node node) {
  assert(node.block()->isLayout());
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

Expression Expression::Parse(const char * textInput) {
  return Expression([](const char * text) {
      EditionPoolLayoutToExpression(Layout::EditionPoolTextToLayout(text));
    }, textInput);
}

Expression Expression::Parse(const Layout * layout) {
  return Expression([](Node node) {
      EditionPoolLayoutToExpression(node);
    }, layout);
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

Layout Expression::toLayout() const {
  return Layout([](Node node) {
      EditionPoolExpressionToLayout(node);
    }, this);
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
