#include "layoutter.h"

#include <poincare/print_float.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Tree *Layoutter::LayoutExpression(Tree *expression) {
  assert(expression->type().isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  // No parentheses on root layout.
  LayoutExpression(layoutParent, expression, false);
  return layoutParent;
}

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

void Layoutter::LayoutText(EditionReference &layoutParent, const char *text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    PushCodePoint(layoutParent, codePoint);
    codePoint = decoder.nextCodePoint();
  }
}

void Layoutter::LayoutBuiltin(EditionReference &layoutParent,
                              Tree *expression) {
  assert(Builtin::IsBuiltin(expression->type()));
  LayoutText(layoutParent, Builtin::Name(expression->type()).mainAlias());
  EditionReference parenthesis =
      SharedEditionPool->push(BlockType::ParenthesisLayout);
  EditionReference newParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    // No parentheses within builtin parameters
    LayoutExpression(newParent, expression->nextNode(), false);
  }
}

void Layoutter::LayoutIntegerHandler(EditionReference &layoutParent,
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

void Layoutter::LayoutInfixOperator(EditionReference &layoutParent,
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
    LayoutExpression(layoutParent, expression->nextNode(), allowParentheses);
  }
}

void Layoutter::LayoutMatrix(EditionReference &layoutParent, Tree *expression) {
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
      LayoutExpression(layoutParent, expression->nextNode());
    }
    PushCodePoint(layoutParent, ']');
  }
  PushCodePoint(layoutParent, ']');
}

void Layoutter::LayoutUnit(EditionReference &layoutParent, Tree *expression) {
  PushCodePoint(layoutParent, '_');
  LayoutText(layoutParent, Unit::GetPrefix(expression)->symbol());
  LayoutText(layoutParent, Unit::GetRepresentative(expression)->rootSymbols());
}

void Layoutter::LayoutPowerOrDivision(EditionReference &layoutParent,
                                      Tree *expression) {
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (type == BlockType::Division) {
    createdLayout = SharedEditionPool->push(BlockType::FractionLayout);
    EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
    LayoutExpression(rack, expression, false);
  } else {
    assert(type == BlockType::Power || type == BlockType::PowerMatrix);
    LayoutExpression(layoutParent, expression);
    createdLayout = SharedEditionPool->push(BlockType::VerticalOffsetLayout);
  }
  EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
  LayoutExpression(rack, expression, false);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Layoutter::LayoutExpression(EditionReference &layoutParent,
                                 Tree *expression, bool allowParentheses) {
  /* TODO: LayoutExpression is a very temporary implementation and must
   *      be improved in the future. */
  assert(Layout::IsHorizontal(layoutParent));
  TypeBlock type = expression->type();

  switch (type) {
    case BlockType::Addition:
    case BlockType::Subtraction: {
      // Add Parentheses if allowed and needed.
      assert(expression->numberOfChildren() > 1);
      EditionReference targetParent = layoutParent;
      if (allowParentheses) {
        EditionReference parenthesis =
            SharedEditionPool->push(BlockType::ParenthesisLayout);
        EditionReference newParent =
            SharedEditionPool->push<BlockType::RackLayout>(0);
        NAry::AddChild(layoutParent, parenthesis);
        targetParent = newParent;
      }
      LayoutInfixOperator(targetParent, expression,
                          (type == BlockType::Addition) ? '+' : '-');
      break;
    }
    case BlockType::Multiplication:
      LayoutInfixOperator(layoutParent, expression, u'×');
      break;
    case BlockType::Power:
    case BlockType::PowerMatrix:
    case BlockType::Division:
      LayoutPowerOrDivision(layoutParent, expression);
      break;
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      LayoutIntegerHandler(layoutParent, Integer::Handler(expression));
      break;
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      EditionReference createdLayout =
          SharedEditionPool->push(BlockType::FractionLayout);
      EditionReference numerator =
          SharedEditionPool->push<BlockType::RackLayout>(0);
      LayoutIntegerHandler(numerator, Rational::Numerator(expression));
      EditionReference denominator =
          SharedEditionPool->push<BlockType::RackLayout>(0);
      LayoutIntegerHandler(denominator, Rational::Denominator(expression));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Decimal:
      LayoutIntegerHandler(layoutParent,
                           Rational::Numerator(expression->nextNode()),
                           Decimal::DecimalOffset(expression));
      expression->nextNode()->removeTree();
    case BlockType::Factorial:
      LayoutExpression(layoutParent, expression->nextNode());
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
    case BlockType::Nonreal:
      LayoutText(layoutParent, "nonreal");
      break;
    case BlockType::Undefined:
      LayoutText(layoutParent, "undef");
      break;
    case BlockType::Matrix:
      LayoutMatrix(layoutParent, expression);
      break;
    case BlockType::Unit:
      LayoutUnit(layoutParent, expression);
      break;
    case BlockType::Float: {
      char buffer[20];
      Poincare::PrintFloat::ConvertFloatToText(
          Float::To(expression), buffer, std::size(buffer),
          Poincare::PrintFloat::k_maxFloatGlyphLength,
          Poincare::PrintFloat::SignificantDecimalDigits<float>(),
          Poincare::Preferences::PrintFloatMode::Decimal);
      LayoutText(layoutParent, buffer);
      break;
    }
    case BlockType::Set:
      PushCodePoint(layoutParent, '{');
      LayoutInfixOperator(layoutParent, expression, ',');
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
        LayoutBuiltin(layoutParent, expression);
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

}  // namespace PoincareJ
