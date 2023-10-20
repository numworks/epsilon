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

static constexpr int OperatorPriority(TypeBlock type) {
  switch (type) {
    case BlockType::Factorial:
      return 0;
    case BlockType::Power:
      return 1;
    case BlockType::Division:
      return 2;
    case BlockType::Multiplication:
      return 3;
    case BlockType::Subtraction:
      return 4;
    case BlockType::Addition:
      return 5;
    case BlockType::Set:
      return 9;
    case BlockType::RackLayout:
      return 10;
    default:
      assert(false);
  }
}

// Commas have no associated block but behave like an operator
static constexpr int k_commaPriority = OperatorPriority(BlockType::Set);

// MaxPriority is to be used when there is no parent that could cause confusion
static constexpr int k_maxPriority = OperatorPriority(BlockType::RackLayout);

Tree *Layoutter::LayoutExpression(Tree *expression, bool linearMode) {
  assert(expression->type().isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  Layoutter layoutter(linearMode);
  layoutter.layoutExpression(layoutParent, expression, k_maxPriority);
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

void Layoutter::layoutText(EditionReference &layoutParent, const char *text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    PushCodePoint(layoutParent, codePoint);
    codePoint = decoder.nextCodePoint();
  }
}

void Layoutter::layoutBuiltin(EditionReference &layoutParent,
                              Tree *expression) {
  assert(Builtin::IsBuiltin(expression->type()));
  layoutText(layoutParent, Builtin::Name(expression->type()).mainAlias());
  EditionReference parenthesis =
      SharedEditionPool->push(BlockType::ParenthesisLayout);
  EditionReference newParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    layoutExpression(newParent, expression->nextNode(), k_commaPriority);
  }
}

void Layoutter::layoutIntegerHandler(EditionReference &layoutParent,
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

void Layoutter::layoutInfixOperator(EditionReference &layoutParent,
                                    Tree *expression, CodePoint op) {
  BlockType type = expression->type();
  int childNumber = expression->numberOfChildren();
  for (int i = 0; i < childNumber; i++) {
    if (i > 0) {
      PushCodePoint(layoutParent, op);
    }
    layoutExpression(layoutParent, expression->nextNode(),
                     OperatorPriority(type));
  }
}

void Layoutter::layoutMatrix(EditionReference &layoutParent, Tree *expression) {
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
      layoutExpression(layoutParent, expression->nextNode(), k_commaPriority);
    }
    PushCodePoint(layoutParent, ']');
  }
  PushCodePoint(layoutParent, ']');
}

void Layoutter::layoutUnit(EditionReference &layoutParent, Tree *expression) {
  PushCodePoint(layoutParent, '_');
  layoutText(layoutParent, Unit::GetPrefix(expression)->symbol());
  layoutText(layoutParent, Unit::GetRepresentative(expression)->rootSymbols());
}

void Layoutter::layoutPowerOrDivision(EditionReference &layoutParent,
                                      Tree *expression) {
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (m_linearMode) {
    if (type == BlockType::Division) {
      layoutExpression(layoutParent, expression,
                       OperatorPriority(BlockType::Multiplication));
      PushCodePoint(layoutParent, '/');
    } else {
      layoutExpression(layoutParent, expression,
                       OperatorPriority(BlockType::Power));
      PushCodePoint(layoutParent, '^');
    }
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    return;
  }
  if (type == BlockType::Division) {
    createdLayout = SharedEditionPool->push(BlockType::FractionLayout);
    EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
    layoutExpression(rack, expression, k_maxPriority);
  } else {
    assert(type == BlockType::Power || type == BlockType::PowerMatrix);
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    createdLayout = SharedEditionPool->push(BlockType::VerticalOffsetLayout);
  }
  EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
  layoutExpression(rack, expression, k_maxPriority);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Layoutter::layoutExpression(EditionReference &layoutParent,
                                 Tree *expression, int parentPriority) {
  assert(Layout::IsHorizontal(layoutParent));
  TypeBlock type = expression->type();

  switch (type) {
    case BlockType::Addition:
    case BlockType::Subtraction:
    case BlockType::Multiplication:
    case BlockType::Power:
    case BlockType::PowerMatrix:
    case BlockType::Division:
    case BlockType::Factorial: {
      EditionReference targetParent = layoutParent;
      // Add Parentheses when necessary
      if (parentPriority < OperatorPriority(type)) {
        EditionReference parenthesis =
            SharedEditionPool->push(BlockType::ParenthesisLayout);
        targetParent = SharedEditionPool->push<BlockType::RackLayout>(0);
        NAry::AddChild(layoutParent, parenthesis);
      }
      switch (type) {
        case BlockType::Addition:
        case BlockType::Multiplication:
          layoutInfixOperator(targetParent, expression,
                              (type == BlockType::Addition) ? '+' : u'×');
          break;
        case BlockType::Power:
        case BlockType::PowerMatrix:
        case BlockType::Division:
          layoutPowerOrDivision(targetParent, expression);
          break;
        case BlockType::Subtraction:
          layoutExpression(targetParent, expression->nextNode(),
                           OperatorPriority(BlockType::Addition));
          PushCodePoint(targetParent, '-');
          layoutExpression(targetParent, expression->nextNode(),
                           OperatorPriority(type));
          break;
        case BlockType::Factorial:
          layoutExpression(targetParent, expression->nextNode(),
                           OperatorPriority(type));
          PushCodePoint(targetParent, '!');
          break;
        default:
          assert(false);
      }
      break;
    }
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      layoutIntegerHandler(layoutParent, Integer::Handler(expression));
      break;
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      EditionReference createdLayout =
          SharedEditionPool->push(BlockType::FractionLayout);
      EditionReference numerator =
          SharedEditionPool->push<BlockType::RackLayout>(0);
      layoutIntegerHandler(numerator, Rational::Numerator(expression));
      EditionReference denominator =
          SharedEditionPool->push<BlockType::RackLayout>(0);
      layoutIntegerHandler(denominator, Rational::Denominator(expression));
      NAry::AddChild(layoutParent, createdLayout);
      break;
    }
    case BlockType::Decimal:
      layoutIntegerHandler(layoutParent,
                           Rational::Numerator(expression->nextNode()),
                           Decimal::DecimalOffset(expression));
      expression->nextNode()->removeTree();
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
      layoutText(layoutParent, "undef");
      break;
    case BlockType::Matrix:
      layoutMatrix(layoutParent, expression);
      break;
    case BlockType::Unit:
      layoutUnit(layoutParent, expression);
      break;
    case BlockType::Float: {
      char buffer[20];
      Poincare::PrintFloat::ConvertFloatToText(
          Float::To(expression), buffer, std::size(buffer),
          Poincare::PrintFloat::k_maxFloatGlyphLength,
          Poincare::PrintFloat::SignificantDecimalDigits<float>(),
          Poincare::Preferences::PrintFloatMode::Decimal);
      layoutText(layoutParent, buffer);
      break;
    }
    case BlockType::Set:
      PushCodePoint(layoutParent, '{');
      layoutInfixOperator(layoutParent, expression, ',');
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
        layoutBuiltin(layoutParent, expression);
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
