#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

void ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                   IntegerHandler handler) {
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  if (handler.strictSign() == StrictSign::Negative) {
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('-'));
  }
  uint64_t value = 0;
  int numberOfDigits = handler.numberOfDigits();
  if (numberOfDigits > 8) {
    assert(false);
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
    return;
  }
  for (int i = numberOfDigits - 1; i >= 0; i--) {
    value = value * (UINT8_MAX + 1) + handler.digits()[i];
  }
  int firstInsertedIndex = layoutParent.numberOfChildren();
  while (value > 0) {
    uint8_t digit = value % 10;
    value /= 10;
    NAry::AddChildAtIndex(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('0' + digit),
        firstInsertedIndex);
  }
}

// Remove expressionReference while converting it to a layout in layoutParent
void ConvertExpressionToLayout(EditionReference layoutParent,
                               EditionReference expressionReference) {
  assert(Layout::IsHorizontal(layoutParent));
  BlockType type = expressionReference.type();
  EditionPool *editionPool = EditionPool::sharedEditionPool();

  // Add Parentheses if needed
  if (layoutParent.numberOfChildren() > 0 &&
      expressionReference.numberOfChildren() > 1 &&
      type != BlockType::Multiplication) {
    EditionReference parenthesis =
        editionPool->push<BlockType::ParenthesisLayout>();
    EditionReference newParent = editionPool->push<BlockType::RackLayout>(0);
    NAry::AddChild(layoutParent, parenthesis);
    return ConvertExpressionToLayout(newParent, expressionReference);
  }

  switch (type) {
    case BlockType::Addition: {
      int childNumber = expressionReference.numberOfChildren();
      for (int i = 0; i < childNumber; i++) {
        if (i > 0) {
          NAry::AddChild(
              layoutParent,
              editionPool->push<BlockType::CodePointLayout, CodePoint>('+'));
        }
        ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      }
      break;
    }
    case BlockType::Multiplication: {
      int childNumber = expressionReference.numberOfChildren();
      for (int i = 0; i < childNumber; i++) {
        if (i > 0) {
          NAry::AddChild(
              layoutParent,
              editionPool->push<BlockType::CodePointLayout, CodePoint>('*'));
        }
        ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      }
      break;
    }
    case BlockType::Power: {
      ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      EditionReference createdLayout =
          editionPool->push<BlockType::VerticalOffsetLayout>();
      ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                                expressionReference.nextNode());
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Subtraction: {
      ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('-'));
      ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      break;
    }
    case BlockType::Division: {
      EditionReference createdLayout =
          editionPool->push<BlockType::FractionLayout>();
      ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                                expressionReference.nextNode());
      ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                                expressionReference.nextNode());
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      ConvertIntegerHandlerToLayout(layoutParent,
                                    Integer::Handler(expressionReference));
      break;
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      EditionReference createdLayout =
          editionPool->push<BlockType::FractionLayout>();
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Numerator(expressionReference));
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Denominator(expressionReference));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Factorial:
      ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('!'));
      break;
    case BlockType::UserSymbol:
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Float:
    case BlockType::Constant:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
    case BlockType::Logarithm:
    default:
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
      break;
  }
  // Children have already been removed.
  expressionReference.removeNode();
}

/* TODO: ConvertExpressionToLayout is a very temporary implementation and must
 * be improved must be improved in the future. */
EditionReference Expression::EditionPoolExpressionToLayout(Node node) {
  assert(node.block()->isExpression());
  EditionReference ref =
      EditionPool::sharedEditionPool()->push<BlockType::RackLayout>(0);
  ConvertExpressionToLayout(ref, EditionReference(node));
  return ref;
}

Expression Expression::Parse(const char *textInput) {
  return Expression(
      [](const char *text) {
        EditionReference layout = Layout::EditionPoolTextToLayout(text);
        Parser::Parse(layout);
        layout.removeTree();
      },
      textInput);
}

Expression Expression::Parse(const Layout *layout) {
  return Expression(
      [](Node node) {
        Parser::Parse(node);
        EditionReference(node).removeTree();
      },
      layout);
}

Expression Expression::CreateSystematicReduction(void *expressionAddress) {
  return Expression(
      [](Node tree) {
        EditionReference(tree).recursivelyEdit([](EditionReference reference) {
          Simplification::SystematicReduction(reference);
        });
      },
      Node(static_cast<const TypeBlock *>(expressionAddress)));
}

Layout Expression::toLayout() const {
  return Layout([](Node node) { EditionPoolExpressionToLayout(node); }, this);
}

float Expression::approximate() const {
  float res;
  send(
      [](const Node tree, void *res) {
        float *result = static_cast<float *>(res);
        *result = Approximation::To<float>(tree);
      },
      &res);
  return res;
}

}  // namespace PoincareJ
