#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

void Expression::ConvertBuiltinToLayout(EditionReference layoutParent,
                                        EditionReference expressionReference) {
  assert(Builtin::IsBuiltin(expressionReference.type()));
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  int i = 0;
  const char *name = Builtin::Name(expressionReference.type()).mainAlias();
  while (name[i] != 0) {
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>(name[i++]));
  }
  EditionReference parenthesis =
      editionPool->push<BlockType::ParenthesisLayout>();
  EditionReference newParent = editionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  EditionReference child = expressionReference.nextNode();
  for (int j = 0; j < expressionReference.numberOfChildren(); j++) {
    if (j != 0) {
      NAry::AddChild(
          newParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(','));
    }
    ConvertExpressionToLayout(newParent, child);
  }
}

void Expression::ConvertIntegerHandlerToLayout(EditionReference layoutParent,
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

void Expression::ConvertInfixOperatorToLayout(
    EditionReference layoutParent, EditionReference expressionReference) {
  BlockType type = expressionReference.type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication ||
         type == BlockType::Subtraction);
  CodePoint codepoint = (type == BlockType::Addition)         ? '+'
                        : (type == BlockType::Multiplication) ? '*'
                                                              : '-';
  int childNumber = expressionReference.numberOfChildren();
  for (int i = 0; i < childNumber; i++) {
    if (i > 0) {
      NAry::AddChild(
          layoutParent,
          EditionPool::sharedEditionPool()
              ->push<BlockType::CodePointLayout, CodePoint>(codepoint));
    }
    ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
  }
}

void Expression::ConvertPowerOrDivisionToLayout(
    EditionReference layoutParent, EditionReference expressionReference) {
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  BlockType type = expressionReference.type();
  EditionReference createdLayout;
  if (type == BlockType::Division) {
    createdLayout = editionPool->push<BlockType::FractionLayout>();
    ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                              expressionReference.nextNode());
  } else {
    assert(type == BlockType::Power);
    ConvertExpressionToLayout(layoutParent, expressionReference.nextNode());
    createdLayout = editionPool->push<BlockType::VerticalOffsetLayout>();
  }
  ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                            expressionReference.nextNode());
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expressionReference while converting it to a layout in layoutParent
void Expression::ConvertExpressionToLayout(
    EditionReference layoutParent, EditionReference expressionReference) {
  /* TODO: ConvertExpressionToLayout is a very temporary implementation and must
   *      be improved in the future. */
  assert(Layout::IsHorizontal(layoutParent));
  BlockType type = expressionReference.type();
  EditionPool *editionPool = EditionPool::sharedEditionPool();

  if (Builtin::IsBuiltin(type)) {
    ConvertBuiltinToLayout(layoutParent, expressionReference);
    expressionReference.removeNode();
    return;
  }

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
    case BlockType::Addition:
    case BlockType::Multiplication:
    case BlockType::Subtraction:
      ConvertInfixOperatorToLayout(layoutParent, expressionReference);
      break;
    case BlockType::Power:
    case BlockType::Division:
      ConvertPowerOrDivisionToLayout(layoutParent, expressionReference);
      break;
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
      assert(*reinterpret_cast<const uint8_t *>(expressionReference.block() +
                                                1) == 1);
      NAry::AddChild(layoutParent,
                     editionPool->push<BlockType::CodePointLayout, CodePoint>(
                         *reinterpret_cast<const char *>(
                             expressionReference.block() + 2)));
      break;
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Float:
    case BlockType::Constant:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      assert(false);
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
      break;
  }
  // Children have already been removed.
  expressionReference.removeNode();
}

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
          Simplification::ShallowSystemReduce(&reference);
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
