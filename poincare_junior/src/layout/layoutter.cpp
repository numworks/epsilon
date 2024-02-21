#include "layoutter.h"

#include <poincare/print_float.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/binary.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/layout/grid.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"
#include "multiplication_symbol.h"

namespace PoincareJ {

// A single token will never need parentheses
static constexpr int k_tokenPriority = -1;

// MaxPriority is to be used when there is no parent that could cause confusion
static constexpr int k_maxPriority = 20;

static constexpr int OperatorPriority(TypeBlock type) {
  switch (type) {
    case BlockType::Factorial:
    case BlockType::PercentSimple:
      return 0;
    case BlockType::Power:
      return 1;
    case BlockType::Division:
      return 2;
    case BlockType::Multiplication:
      return 3;
    case BlockType::PercentAddition:
      return 4;
    case BlockType::Opposite:
    // Opposite could be higher but we prefer to display 2^(-1) instead of 2^-1
    case BlockType::Subtraction:
      return 5;
    case BlockType::Addition:
      return 6;

    case BlockType::LogicalNot:
      return 12;
    case BlockType::LogicalAnd:
      return 13;
    // TODO PCJ force parentheses on equality
    case BlockType::LogicalOr:
    case BlockType::LogicalXor:
      return 14;
    case BlockType::Point:
    case BlockType::Set:
    case BlockType::List:
      return 19;
    case BlockType::RackLayout:
      return 20;
    default:
      return k_tokenPriority;
  }
}

// Commas have no associated block but behave like an operator
static constexpr int k_commaPriority = OperatorPriority(BlockType::Set);

Tree *Layoutter::LayoutExpression(Tree *expression, bool linearMode,
                                  int numberOfSignificantDigits) {
  assert(expression->isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  Layoutter layoutter(linearMode, numberOfSignificantDigits);
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
  assert(Builtin::IsReservedFunction(expression));
  const Builtin *builtin = Builtin::GetReservedFunction(expression);
  if (m_linearMode || !builtin->has2DLayout()) {
    // Built "builtin(child1, child2)"
    layoutFunctionCall(layoutParent, expression,
                       builtin->aliases()->mainAlias());
  } else {
    // Built 2D layout associated with builtin
    const BuiltinWithLayout *builtinWithLayout =
        static_cast<const BuiltinWithLayout *>(builtin);
    EditionReference layout = SharedEditionPool->push(
        static_cast<BlockType>(builtinWithLayout->layoutType()));
    layoutChildrenAsRacks(layoutParent, expression);
    NAry::AddChild(layoutParent, layout);
  }
}

void Layoutter::layoutFunctionCall(EditionReference &layoutParent,
                                   Tree *expression, const char *name) {
  layoutText(layoutParent, name);
  EditionReference parenthesis =
      SharedEditionPool->push<BlockType::ParenthesisLayout>(false, false);
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

void Layoutter::layoutChildrenAsRacks(EditionReference &layoutParent,
                                      Tree *expression) {
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    EditionReference newParent =
        SharedEditionPool->push<BlockType::RackLayout>(0);
    layoutExpression(newParent, expression->nextNode(), k_maxPriority);
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
    DivisionResult result =
        IntegerHandler::Division(Integer::Handler(value), IntegerHandler(10));
    uint8_t digit = Integer::Handler(result.remainder);
    assert(result.remainder > result.quotient);
    result.remainder->removeTree();
    MoveTreeOverTree(value, result.quotient);
    InsertCodePointAt(layoutParent, '0' + digit, firstInsertedIndex);
    if (--decimalOffset == 0) {
      InsertCodePointAt(layoutParent, '.', firstInsertedIndex);
    }
  } while (!value->isZero() && decimalOffset <= 0);
  value->removeTree();
}

void Layoutter::layoutInfixOperator(EditionReference &layoutParent,
                                    Tree *expression, CodePoint op) {
  BlockType type = expression->type();
  int childNumber = expression->numberOfChildren();
  for (int childIndex = 0; childIndex < childNumber; childIndex++) {
    Tree *child = expression->nextNode();
    if (childIndex > 0 && !(op == '+' && child->isOpposite())) {
      PushCodePoint(layoutParent, op);
    }
    layoutExpression(layoutParent, child, OperatorPriority(type));
  }
}

void Layoutter::layoutMatrix(EditionReference &layoutParent, Tree *expression) {
  if (!m_linearMode) {
    EditionReference layout = expression->cloneNode();
    *layout->block() = BlockType::MatrixLayout;
    layoutChildrenAsRacks(layoutParent, expression);
    NAry::AddChild(layoutParent, layout);
    Grid *grid = Grid::From(layout);
    grid->addEmptyColumn();
    grid->addEmptyRow();
    return;
  }
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
  layoutText(layoutParent, Units::Unit::GetPrefix(expression)->symbol());
  layoutText(layoutParent,
             Units::Unit::GetRepresentative(expression)->rootSymbols());
}

void Layoutter::layoutPowerOrDivision(EditionReference &layoutParent,
                                      Tree *expression) {
  BlockType type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  EditionReference createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (m_linearMode) {
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    PushCodePoint(layoutParent, type == BlockType::Division ? '/' : '^');
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
    createdLayout = KVertOffL->cloneNode();
  }
  EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
  layoutExpression(rack, expression, k_maxPriority);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Layoutter::layoutExpression(EditionReference &layoutParentRef,
                                 Tree *expression, int parentPriority) {
  assert(layoutParentRef->isRackLayout());
  TypeBlock type = expression->type();

  EditionReference layoutParent;

  // Add Parentheses if necessary
  if (parentPriority < OperatorPriority(type)) {
    EditionReference parenthesis = KParenthesisL(KRackL())->clone();
    NAry::AddChild(layoutParentRef, parenthesis);
    layoutParent = parenthesis->child(0);
  } else {
    layoutParent = layoutParentRef;
  }

  switch (type) {
    case BlockType::Addition:
      layoutInfixOperator(layoutParent, expression, '+');
      break;
    case BlockType::Multiplication:
      /* TODO PCJ: Add small margins when units are present */
      layoutInfixOperator(
          layoutParent, expression,
          m_linearMode ? CodePoint(u'×') : MultiplicationSymbol(expression));
      break;
    case BlockType::Power:
    case BlockType::PowerMatrix:
    case BlockType::Division:
      layoutPowerOrDivision(layoutParent, expression);
      break;
    case BlockType::Subtraction:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(BlockType::Addition));
      PushCodePoint(layoutParent, '-');
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case BlockType::Opposite: {
      PushCodePoint(layoutParent, '-');
      // Add extra parentheses for -1^2 -> -(1^2) but not for -x^2
      bool addExtraParenthesis =
          expression->child(0)->isPower() &&
          !expression->child(0)->child(0)->isUserSymbol();
      layoutExpression(
          layoutParent, expression->nextNode(),
          addExtraParenthesis ? k_tokenPriority : OperatorPriority(type));
      break;
    }
    case BlockType::Factorial:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '!');
      break;
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '%');
      break;
    case BlockType::PercentAddition:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, UCodePointNorthEastArrow);
      // continue
    case BlockType::PercentSimple:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '%');
      break;
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
#ifndef POINCARE_MEMORY_TREE_LOG
      assert(!Rational::Sign(expression).isStrictlyNegative());
#endif
      layoutIntegerHandler(layoutParent, Integer::Handler(expression));
      break;
    case BlockType::IntegerNegBig:
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
#if POINCARE_MEMORY_TREE_LOG
      layoutIntegerHandler(layoutParent, Rational::Numerator(expression));
      PushCodePoint(layoutParent, '/');
      layoutIntegerHandler(layoutParent, Rational::Denominator(expression));
#else
      // Expression should be beautifyied before layoutting
      assert(false);
#endif
      break;
    }
    case BlockType::Decimal:
      layoutIntegerHandler(layoutParent,
                           Rational::Numerator(expression->nextNode()),
                           Decimal::DecimalOffset(expression));
      expression->nextNode()->removeTree();
    case BlockType::Pi:
      PushCodePoint(layoutParent, u'π');
      break;
    case BlockType::ExponentialE:
      PushCodePoint(layoutParent, 'e');
      break;
    case BlockType::ComplexI:
      PushCodePoint(layoutParent, 'i');
      break;
    case BlockType::UserSymbol:
      assert(Symbol::Length(expression) == 1);
      PushCodePoint(layoutParent, *Symbol::NonNullTerminatedName(expression));
      break;
    case BlockType::Infinity:
    case BlockType::Nonreal:
    case BlockType::Undefined:
      layoutText(layoutParent,
                 Builtin::SpecialIdentifierName(type).mainAlias());
      break;
    case BlockType::Matrix:
      layoutMatrix(layoutParent, expression);
      break;
    case BlockType::Unit:
      layoutUnit(layoutParent, expression);
      break;
    case BlockType::DoubleFloat:
    case BlockType::SingleFloat: {
      char buffer[20];
      Poincare::PrintFloat::ConvertFloatToText(
          Float::To(expression), buffer, std::size(buffer),
          Poincare::PrintFloat::k_maxFloatGlyphLength,
          m_numberOfSignificantDigits != -1 ? m_numberOfSignificantDigits
          : type == BlockType::SingleFloat
              ? Poincare::PrintFloat::SignificantDecimalDigits<float>()
              : Poincare::PrintFloat::SignificantDecimalDigits<double>(),
          Poincare::Preferences::PrintFloatMode::Decimal);
      layoutText(layoutParent, buffer);
      break;
    }
    case BlockType::True:
      layoutText(layoutParent, BuiltinsAliases::k_trueAliases.mainAlias());
      break;
    case BlockType::False:
      layoutText(layoutParent, BuiltinsAliases::k_falseAliases.mainAlias());
      break;
    case BlockType::LogicalAnd:
    case BlockType::LogicalOr:
    case BlockType::LogicalXor:
    case BlockType::LogicalNot:
      if (!type.isLogicalNot()) {
        layoutExpression(layoutParent, expression->nextNode(),
                         OperatorPriority(type));
        PushCodePoint(layoutParent, ' ');
      }
      layoutText(layoutParent, Binary::OperatorName(type));
      PushCodePoint(layoutParent, ' ');
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    // TODO make list and set different
    case BlockType::List:
    case BlockType::Set:
    case BlockType::Point:
      PushCodePoint(layoutParent, type.isPoint() ? '(' : '{');
      layoutInfixOperator(layoutParent, expression, ',');
      PushCodePoint(layoutParent, type.isPoint() ? ')' : '}');
      break;
#if POINCARE_MEMORY_TREE_LOG
    case BlockType::Placeholder: {
      PushCodePoint(layoutParent, 'K');
      if (Placeholder::NodeToFilter(expression) ==
          Placeholder::Filter::AnyTrees) {
        PushCodePoint(layoutParent, 'T');
      }
      uint8_t offset = Placeholder::NodeToTag(expression);
      char name = 'A' + offset;
      PushCodePoint(layoutParent, name);
      break;
    }
    case BlockType::Exponential:
      layoutFunctionCall(layoutParent, expression, "exp");
      break;
    case BlockType::Variable: {
      uint8_t offset = Variables::Id(expression);
      char name = 'a' + offset;
      // Skip e and i
      if (name >= 'e') {
        name++;
      }
      if (name >= 'i') {
        name++;
      }
      PushCodePoint(layoutParent, name);
      break;
    }
#endif
    case BlockType::Polynomial:
    default:
      if (Builtin::IsReservedFunction(expression)) {
        if (type.isParametric()) {
          // Move sub-expression first
          expression->child(0)->moveTreeBeforeNode(
              expression->child(expression->numberOfChildren() - 1));
        }
        layoutBuiltin(layoutParent, expression);
      } else {
#if POINCARE_MEMORY_TREE_LOG
        layoutFunctionCall(
            layoutParent, expression,
            TypeBlock::names[static_cast<uint8_t>(*expression->block())]);
#else
        // TODO: Handle missing BlockTypes
        assert(false);
        PushCodePoint(layoutParent, '?');
#endif
        break;
      }
  }
  // Children have already been removed.
  expression->removeNode();
}

}  // namespace PoincareJ
