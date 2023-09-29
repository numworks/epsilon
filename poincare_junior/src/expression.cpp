#include <poincare/print_float.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include "poincare_junior/src/expression/symbol.h"

namespace PoincareJ {

static void PushCodePoint(Tree *layout, CodePoint codePoint) {
  NAry::AddChild(layout,
                 SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(
                     codePoint));
}

static void InsertCodePointAt(Tree *layout, CodePoint codePoint, int index) {
  NAry::AddChildAtIndex(
      layout,
      SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(codePoint),
      index);
}

void Expression::ConvertTextToLayout(EditionReference layoutParent,
                                     const char *text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    PushCodePoint(layoutParent, codePoint);
    codePoint = decoder.nextCodePoint();
  }
}

void Expression::ConvertBuiltinToLayout(EditionReference layoutParent,
                                        Tree *expression) {
  assert(Builtin::IsBuiltin(expression->type()));
  ConvertTextToLayout(layoutParent,
                      Builtin::Name(expression->type()).mainAlias());
  EditionReference parenthesis =
      SharedEditionPool->push<BlockType::ParenthesisLayout>();
  EditionReference newParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    // No parentheses within builtin parameters
    ConvertExpressionToLayout(newParent, expression->nextNode(), false);
  }
}

void Expression::ConvertIntegerHandlerToLayout(EditionReference layoutParent,
                                               IntegerHandler handler,
                                               int decimalOffset) {
  if (handler.strictSign() == StrictSign::Negative) {
    PushCodePoint(layoutParent, '-');
  }
  handler.setSign(NonStrictSign::Positive);
  int firstInsertedIndex = layoutParent->numberOfChildren();
  /* We can't manipulate an IntegerHandler in a workingBuffer since we're
   * pushing layouts on the EditionPool at each steps. Value is therefore
   * temporarily stored and updated on the EditionPool. */
  EditionReference value = handler.pushOnEditionPool();
  do {
    std::pair<PoincareJ::Tree *, PoincareJ::Tree *> result =
        IntegerHandler::Division(Integer::Handler(value), IntegerHandler(10));
    uint8_t digit = Integer::Handler(result.second);
    assert(result.second > result.first);
    result.second->removeTree();
    MoveTreeOverTree(value, result.first);
    InsertCodePointAt(layoutParent, '0' + digit, firstInsertedIndex);
    if (--decimalOffset == 0) {
      InsertCodePointAt(layoutParent, '.', firstInsertedIndex);
    }
  } while (!Number::IsZero(value) && decimalOffset <= 0);
  value->removeTree();
}

void Expression::ConvertInfixOperatorToLayout(EditionReference layoutParent,
                                              Tree *expression, CodePoint op) {
  BlockType type = expression->type();
  int childNumber = expression->numberOfChildren();
  for (int i = 0; i < childNumber; i++) {
    if (i > 0) {
      PushCodePoint(layoutParent, op);
    }
    // 2*(x+y) or x-(y+z)
    bool allowParentheses = (type == BlockType::Multiplication) ||
                            ((type == BlockType::Subtraction) && (i == 1));
    ConvertExpressionToLayout(layoutParent, expression->nextNode(),
                              allowParentheses);
  }
}

void Expression::ConvertMatrixToLayout(EditionReference layoutParent,
                                       Tree *expression) {
  // TODO : matrix layout
  PushCodePoint(layoutParent, '[');
  int cols = Matrix::NumberOfColumns(expression);
  int rows = Matrix::NumberOfRows(expression);
  for (int row = 0; row < rows; row++) {
    PushCodePoint(layoutParent, '[');
    for (int col = 0; col < cols; col++) {
      if (col > 0) {
        PushCodePoint(layoutParent, ',');
      }
      ConvertExpressionToLayout(layoutParent, expression->nextNode());
    }
    PushCodePoint(layoutParent, ']');
  }
  PushCodePoint(layoutParent, ']');
}

void Expression::ConvertUnitToLayout(EditionReference layoutParent,
                                     Tree *expression) {
  PushCodePoint(layoutParent, '_');
  ConvertTextToLayout(layoutParent, Unit::GetPrefix(expression)->symbol());
  ConvertTextToLayout(layoutParent,
                      Unit::GetRepresentative(expression)->rootSymbols());
}

void Expression::ConvertPowerOrDivisionToLayout(EditionReference layoutParent,
                                                Tree *expression) {
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (type == BlockType::Division) {
    createdLayout = SharedEditionPool->push<BlockType::FractionLayout>();
    ConvertExpressionToLayout(SharedEditionPool->push<BlockType::RackLayout>(0),
                              expression, false);
  } else {
    assert(type == BlockType::Power || type == BlockType::PowerMatrix);
    ConvertExpressionToLayout(layoutParent, expression);
    createdLayout = SharedEditionPool->push<BlockType::VerticalOffsetLayout>();
  }
  ConvertExpressionToLayout(SharedEditionPool->push<BlockType::RackLayout>(0),
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
  TypeBlock type = expression->type();

  switch (type) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      // Add Parentheses if allowed and needed.
      assert(expression->numberOfChildren() > 1);
      if (allowParentheses) {
        EditionReference parenthesis =
            SharedEditionPool->push<BlockType::ParenthesisLayout>();
        EditionReference newParent =
            SharedEditionPool->push<BlockType::RackLayout>(0);
        NAry::AddChild(layoutParent, parenthesis);
        layoutParent = newParent;
      }
      // continue
    case BlockType::Multiplication: {
      CodePoint codepoint = (type == BlockType::Addition)         ? '+'
                            : (type == BlockType::Multiplication) ? '*'
                                                                  : '-';
      ConvertInfixOperatorToLayout(layoutParent, expression, codepoint);
      break;
    }
    case BlockType::Power:
    case BlockType::PowerMatrix:
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
          SharedEditionPool->push<BlockType::FractionLayout>();
      ConvertIntegerHandlerToLayout(
          SharedEditionPool->push<BlockType::RackLayout>(0),
          Rational::Numerator(expression));
      ConvertIntegerHandlerToLayout(
          SharedEditionPool->push<BlockType::RackLayout>(0),
          Rational::Denominator(expression));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Decimal:
      ConvertIntegerHandlerToLayout(layoutParent,
                                    Rational::Numerator(expression->nextNode()),
                                    Decimal::DecimalOffset(expression));
      expression->nextNode()->removeTree();
    case BlockType::Factorial:
      ConvertExpressionToLayout(layoutParent, expression->nextNode());
      PushCodePoint(layoutParent, '!');
      break;
    case BlockType::Constant:
      PushCodePoint(layoutParent,
                    Constant::ToCodePoint(Constant::Type(expression)));
      break;
    case BlockType::UserSymbol:
      assert(Symbol::Length(expression) == 1);
      PushCodePoint(layoutParent, *Symbol::NonNullTerminatedName(expression));
      break;
    case BlockType::Undefined:
      ConvertTextToLayout(layoutParent, "undef");
      break;
    case BlockType::Matrix:
      ConvertMatrixToLayout(layoutParent, expression);
      break;
    case BlockType::Unit:
      ConvertUnitToLayout(layoutParent, expression);
      break;
    case BlockType::Float: {
      char buffer[20];
      Poincare::PrintFloat::ConvertFloatToText(
          Float::To(expression), buffer, std::size(buffer),
          Poincare::PrintFloat::k_maxFloatGlyphLength,
          Poincare::PrintFloat::SignificantDecimalDigits<float>(),
          Poincare::Preferences::PrintFloatMode::Decimal);
      ConvertTextToLayout(layoutParent, buffer);
      break;
    }
    case BlockType::Set:
      PushCodePoint(layoutParent, '{');
      ConvertInfixOperatorToLayout(layoutParent, expression, ',');
      PushCodePoint(layoutParent, '}');
      break;
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      if (Builtin::IsBuiltin(type)) {
        if (type.isParametric()) {
          // Move sub-expression first
          expression->child(0)->moveTreeBeforeNode(
              expression->child(expression->numberOfChildren() - 1));
        }
        ConvertBuiltinToLayout(layoutParent, expression);
      } else {
        // TODO: Handle missing BlockTypes
        assert(false);
        PushCodePoint(layoutParent, '?');
        break;
      }
  }
  // Children have already been removed.
  expression->removeNode();
}

EditionReference Expression::EditionPoolExpressionToLayout(Tree *expression) {
  assert(expression->type().isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
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
