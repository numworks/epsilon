#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

void Expression::ConvertBuiltinToLayout(EditionReference layoutParent,
                                        Tree *expression) {
  assert(Builtin::IsBuiltin(expression->type()));
  UTF8Decoder decoder(Builtin::Name(expression->type()).mainAlias());
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    NAry::AddChild(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>(codePoint));
    codePoint = decoder.nextCodePoint();
  }
  EditionReference parenthesis =
      editionPool->push<BlockType::ParenthesisLayout>();
  EditionReference newParent = editionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j != 0) {
      NAry::AddChild(
          newParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(','));
    }
    // No parentheses within builtin parameters
    ConvertExpressionToLayout(newParent, expression->nextNode(), false);
  }
}

void Expression::ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                               IntegerHandler handler) {
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
  int firstInsertedIndex = layoutParent->numberOfChildren();
  do {
    uint8_t digit = value % 10;
    value /= 10;
    NAry::AddChildAtIndex(
        layoutParent,
        editionPool->push<BlockType::CodePointLayout, CodePoint>('0' + digit),
        firstInsertedIndex);
  } while (value > 0);
}

void Expression::ConvertInfixOperatorToLayout(EditionReference layoutParent,
                                              Tree *expression) {
  BlockType type = expression->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication ||
         type == BlockType::Subtraction);
  CodePoint codepoint = (type == BlockType::Addition)         ? '+'
                        : (type == BlockType::Multiplication) ? '*'
                                                              : '-';
  int childNumber = expression->numberOfChildren();
  for (int i = 0; i < childNumber; i++) {
    if (i > 0) {
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(codepoint));
    }
    // 2*(x+y) or x-(y+z)
    bool allowParentheses = (type == BlockType::Multiplication) ||
                            ((type == BlockType::Subtraction) && (i == 1));
    ConvertExpressionToLayout(layoutParent, expression->nextNode(),
                              allowParentheses);
  }
}

void Expression::ConvertPowerOrDivisionToLayout(EditionReference layoutParent,
                                                Tree *expression) {
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (type == BlockType::Division) {
    createdLayout = editionPool->push<BlockType::FractionLayout>();
    ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                              expression, false);
  } else {
    assert(type == BlockType::Power);
    ConvertExpressionToLayout(layoutParent, expression);
    createdLayout = editionPool->push<BlockType::VerticalOffsetLayout>();
  }
  ConvertExpressionToLayout(editionPool->push<BlockType::RackLayout>(0),
                            expression, false);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Expression::ConvertExpressionToLayout(EditionReference layoutParent,
                                           Tree *expression,
                                           bool allowParentheses) {
  /* TODO: ConvertExpressionToLayout is a very temporary implementation and must
   *      be improved in the future. */
  assert(Layout::IsHorizontal(layoutParent));
  BlockType type = expression->type();

  switch (type) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      // Add Parentheses if allowed and needed.
      assert(expression->numberOfChildren() > 1);
      if (allowParentheses) {
        EditionReference parenthesis =
            editionPool->push<BlockType::ParenthesisLayout>();
        EditionReference newParent =
            editionPool->push<BlockType::RackLayout>(0);
        NAry::AddChild(layoutParent, parenthesis);
        layoutParent = newParent;
      }
      // continue
    case BlockType::Multiplication:
      ConvertInfixOperatorToLayout(layoutParent, expression);
      break;
    case BlockType::Power:
    case BlockType::Division:
      ConvertPowerOrDivisionToLayout(layoutParent, expression);
      break;
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      ConvertIntegerHandlerToLayout(layoutParent, Integer::Handler(expression));
      break;
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      EditionReference createdLayout =
          editionPool->push<BlockType::FractionLayout>();
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Numerator(expression));
      ConvertIntegerHandlerToLayout(editionPool->push<BlockType::RackLayout>(0),
                                    Rational::Denominator(expression));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Factorial:
      ConvertExpressionToLayout(layoutParent, expression->nextNode());
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>('!'));
      break;
    case BlockType::Constant:
      NAry::AddChild(layoutParent,
                     editionPool->push<BlockType::CodePointLayout, CodePoint>(
                         Constant::ToCodePoint(Constant::Type(expression))));
      break;
    case BlockType::UserSymbol:
      assert(*reinterpret_cast<const uint8_t *>(expression->block() + 1) == 1);
      NAry::AddChild(
          layoutParent,
          editionPool->push<BlockType::CodePointLayout, CodePoint>(
              *reinterpret_cast<const char *>(expression->block() + 2)));
      break;
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Float:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      if (Builtin::IsBuiltin(type)) {
        ConvertBuiltinToLayout(layoutParent, expression);
      } else {
        assert(false);
        NAry::AddChild(
            layoutParent,
            editionPool->push<BlockType::CodePointLayout, CodePoint>('?'));
        break;
      }
  }
  // Children have already been removed.
  expression->removeNode();
}

EditionReference Expression::EditionPoolExpressionToLayout(Tree *expression) {
  assert(expression->block()->isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent = editionPool->push<BlockType::RackLayout>(0);
  // No parentheses on root layout.
  ConvertExpressionToLayout(layoutParent, expression, false);
  return layoutParent;
}

Expression Expression::Parse(const char *textInput) {
  return Expression(
      [](const char *text) {
        EditionReference layout = Layout::EditionPoolTextToLayout(text);
        Parser::Parse(layout);
        layout->removeTree();
      },
      textInput);
}

Expression Expression::Parse(const Layout *layout) {
  return Expression(
      [](Tree *node) {
        Parser::Parse(node);
        node->removeTree();
      },
      layout);
}

Expression Expression::CreateSimplifyReduction(void *expressionAddress) {
  return Expression(
      [](Tree *tree) {
        EditionReference reference(tree);
        Simplification::Simplify(reference);
      },
      Tree::FromBlocks(static_cast<const TypeBlock *>(expressionAddress)));
}

Layout Expression::toLayout() const {
  return Layout([](Tree *node) { EditionPoolExpressionToLayout(node); }, this);
}

float Expression::approximate() const {
  float res;
  send(
      [](const Tree *tree, void *res) {
        float *result = static_cast<float *>(res);
        *result = Approximation::To<float>(tree);
      },
      &res);
  return res;
}

}  // namespace PoincareJ
