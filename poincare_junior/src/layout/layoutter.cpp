#include "layoutter.h"

#include <poincare/print_float.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/binary.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
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

using Poincare::Preferences;

namespace PoincareJ {

static constexpr int k_forceParenthesis = -2;

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
    case BlockType::MixedFraction:
      return 6;

    case BlockType::Equal:
    case BlockType::NotEqual:
    case BlockType::InferiorEqual:
    case BlockType::Inferior:
    case BlockType::SuperiorEqual:
      return 11;

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
      return 18;
    case BlockType::Store:
    case BlockType::UnitConversion:
      // 2,3→x is read as (2,3)→x not 2,(3→x) (even if invalid)
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
                                  int numberOfSignificantDigits,
                                  Preferences::PrintFloatMode floatMode) {
  assert(expression->isExpression());
  /* expression lives before layoutParent in the EditionPool and will be
   * destroyed in the process. An EditionReference is necessary to keep track of
   * layoutParent's root. */
  EditionReference layoutParent =
      SharedEditionPool->push<BlockType::RackLayout>(0);
  Layoutter layoutter(linearMode, false, numberOfSignificantDigits, floatMode);
  layoutter.m_addSeparators =
      !linearMode && layoutter.requireSeparators(expression);
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

void Layoutter::addSeparator(Tree *layoutParent) {
  if (!m_addSeparators) {
    return;
  }
  assert(!m_linearMode);
  NAry::AddChild(layoutParent, KOperatorSeparatorL->clone());
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
    if (expression->isParametric()) {
      // Move sub-expression first
      expression->child(0)->moveTreeBeforeNode(
          expression->child(expression->numberOfChildren() - 1));
    }
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
  Tree *rack = KRackL()->clone();
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
    InsertCodePointAt(rack, '0' + digit, 0);
    if (--decimalOffset == 0) {
      InsertCodePointAt(rack, '.', 0);
      if (value->isZero()) {
        // TODO insert 0 before . if nothing else is to come
        InsertCodePointAt(rack, '0', 0);
      }
    }
  } while (!(value->isZero() && decimalOffset <= 0));
  value->removeTree();
  AddThousandSeparators(rack);
  NAry::AddOrMergeChild(layoutParent, rack);
}

void Layoutter::layoutInfixOperator(EditionReference &layoutParent,
                                    Tree *expression, CodePoint op,
                                    bool multiplication) {
  BlockType type = expression->type();
  int childNumber = expression->numberOfChildren();
  bool previousWasUnit = false;
  for (int childIndex = 0; childIndex < childNumber; childIndex++) {
    Tree *child = expression->nextNode();
    bool isUnit = Units::Unit::IsUnitOrPowerOfUnit(child);
    if (childIndex > 0) {
      if (!m_linearMode && multiplication && isUnit) {
        if (!previousWasUnit) {
          if (Units::Unit::ForceMarginLeftOfUnit(child)) {
            addSeparator(layoutParent);
          }
        } else {
          PushCodePoint(layoutParent, UCodePointMiddleDot);
        }
        layoutExpression(layoutParent, child, OperatorPriority(type));
        previousWasUnit = isUnit;
        continue;
      }
      addSeparator(layoutParent);
      if (op != UCodePointNull && !(op == '+' && child->isOpposite())) {
        PushCodePoint(layoutParent, op);
        addSeparator(layoutParent);
      }
    }
    layoutExpression(layoutParent, child, OperatorPriority(type));
    previousWasUnit = isUnit;
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
  // TODO PCJ ask the context whether to add an underscore
  if (m_linearMode) {
    PushCodePoint(layoutParent, '_');
  }
  layoutText(layoutParent, Units::Unit::GetPrefix(expression)->symbol());
  layoutText(
      layoutParent,
      Units::Unit::GetRepresentative(expression)->rootSymbols().mainAlias());
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
    createdLayout = KSuperscriptL->cloneNode();
  }
  EditionReference rack = SharedEditionPool->push<BlockType::RackLayout>(0);
  layoutExpression(rack, expression, k_maxPriority);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Layoutter::layoutExpression(EditionReference &layoutParent,
                                 Tree *expression, int parentPriority) {
  assert(layoutParent->isRackLayout());
  TypeBlock type = expression->type();

  // Add Parentheses if necessary
  if (parentPriority < OperatorPriority(type) &&
      !(type.isPoint() || type.isList())) {
    EditionReference parenthesis = KParenthesisL(KRackL())->clone();
    NAry::AddChild(layoutParent, parenthesis);
    EditionReference rack = parenthesis->child(0);
    return layoutExpression(rack, expression, k_maxPriority);
  }

  switch (type) {
    case BlockType::Addition: {
      CodePoint op =
          ImplicitAddition(expression) ? UCodePointNull : CodePoint('+');
      layoutInfixOperator(layoutParent, expression, op);
      break;
    }
    case BlockType::Multiplication:
      /* TODO PCJ: Add small margins when units are present */
      layoutInfixOperator(
          layoutParent, expression,
          m_linearMode ? CodePoint(u'×') : MultiplicationSymbol(expression),
          true);
      break;
    case BlockType::Power:
    case BlockType::PowerMatrix:
    case BlockType::Division:
      layoutPowerOrDivision(layoutParent, expression);
      break;
    case BlockType::Subtraction:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(BlockType::Addition));
      addSeparator(layoutParent);
      PushCodePoint(layoutParent, '-');
      addSeparator(layoutParent);
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
          addExtraParenthesis ? k_forceParenthesis : OperatorPriority(type));
      break;
    }
    case BlockType::Factorial:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '!');
      break;
    case BlockType::PercentAddition:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      if (expression->nextNode()->isOpposite()) {
        PushCodePoint(layoutParent, UCodePointSouthEastArrow);
        expression->nextNode()->removeNode();
      } else {
        PushCodePoint(layoutParent, UCodePointNorthEastArrow);
      }
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
    case BlockType::Derivative:
    case BlockType::NthDerivative:
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        EditionReference layout =
            (type.isDerivative() ? KDerivativeL : KNthDerivativeL)->cloneNode();
        if (type.isNthDerivative()) {
          // Handle the peculiar order of nth-derivative layout
          // TODO fix order in derivative layout instead
          expression->child(2)->moveTreeBeforeNode(expression->child(3));
        }
        layoutChildrenAsRacks(layout, expression);
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case BlockType::Binomial:
    case BlockType::Permute:
      if (m_linearMode ||
          Preferences::SharedPreferences()->combinatoricSymbols() ==
              Preferences::CombinatoricSymbols::Default) {
        layoutBuiltin(layoutParent, expression);
      } else {
        EditionReference layout = SharedEditionPool->push(
            type.isBinomial() ? BlockType::PtBinomialLayout
                              : BlockType::PtPermuteLayout);
        layoutChildrenAsRacks(layoutParent, expression);
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case BlockType::Logarithm: {
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
        break;
      }
      constexpr const char *log =
          Builtin::GetReservedFunction(BlockType::Logarithm)
              ->aliases()
              ->mainAlias();
      bool nlLog = Preferences::SharedPreferences()->logarithmBasePosition() ==
                   Preferences::LogarithmBasePosition::TopLeft;
      if (!nlLog) {
        layoutText(layoutParent, log);
      }
      // Base
      EditionReference layout =
          nlLog ? KPrefixSuperscriptL->cloneNode() : KSubscriptL->cloneNode();
      EditionReference newParent = KRackL()->clone();
      layoutExpression(newParent, expression->nextNode()->nextTree(),
                       k_maxPriority);
      NAry::AddChild(layoutParent, layout);
      if (nlLog) {
        layoutText(layoutParent, log);
      }
      // Value
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParenthesis);
      break;
    }
    case BlockType::MixedFraction:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      if (m_linearMode) {
        // TODO PCJ make sure the serializer makes the distinction too
        PushCodePoint(layoutParent, ' ');
      }
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case BlockType::Pi:
      PushCodePoint(layoutParent, u'π');
      break;
    case BlockType::ExponentialE:
      PushCodePoint(layoutParent, 'e');
      break;
    case BlockType::ComplexI:
      PushCodePoint(layoutParent, 'i');
      break;
    case BlockType::PhysicalConstant:
      layoutText(layoutParent,
                 Constant::Info(expression).m_aliasesList.mainAlias());
      break;
    case BlockType::UserSymbol:
    case BlockType::UserSequence:
    case BlockType::UserFunction: {
      constexpr int bufferSize = sizeof(CodePoint) * Symbol::k_maxNameSize;
      char buffer[bufferSize];
      Symbol::GetName(expression, buffer, std::size(buffer));
      layoutText(layoutParent, buffer);
      if (type.isUserFunction()) {
        // minimum priority to force parentheses
        layoutExpression(layoutParent, expression->nextNode(),
                         k_forceParenthesis);
      }
      if (type.isUserSequence()) {
        if (m_linearMode) {
          PushCodePoint(layoutParent, '_');
          layoutExpression(layoutParent, expression->nextNode(),
                           k_forceParenthesis);
        } else {
          EditionReference layout = KSubscriptL->cloneNode();
          layoutChildrenAsRacks(layoutParent, expression);
          NAry::AddChild(layoutParent, layout);
        }
      }
      break;
    }
    case BlockType::Infinity:
    case BlockType::Nonreal:
    case BlockType::Undefined:
      layoutText(layoutParent,
                 Builtin::SpecialIdentifierName(type).mainAlias());
      break;
    case BlockType::Empty:
      break;
    case BlockType::Matrix:
      layoutMatrix(layoutParent, expression);
      break;
    case BlockType::Piecewise:
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        int rows = (expression->numberOfChildren() + 1) / 2;
        EditionReference layout =
            SharedEditionPool->push(BlockType::PiecewiseLayout);
        SharedEditionPool->push(rows + 1);
        SharedEditionPool->push(2);
        layoutChildrenAsRacks(layoutParent, expression);
        // Placeholders
        if (expression->numberOfChildren() % 2 == 1) {
          KRackL()->clone();
        }
        KRackL()->clone();
        KRackL()->clone();
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case BlockType::Unit:
      layoutUnit(layoutParent, expression);
      break;
    case BlockType::Decimal:
    case BlockType::SingleFloat:
    case BlockType::DoubleFloat: {
      char buffer[100];
      if (type.isDecimal()) {
        IntegerHandler::ConvertDecimalToText(expression, buffer,
                                             std::size(buffer), m_floatMode,
                                             m_numberOfSignificantDigits);
        expression->nextNode()->removeTree();
      } else {
        Poincare::PrintFloat::ConvertFloatToText(
            FloatNode::To(expression), buffer, std::size(buffer),
            Poincare::PrintFloat::k_maxFloatGlyphLength,
            m_numberOfSignificantDigits != -1 ? m_numberOfSignificantDigits
            : type == BlockType::SingleFloat
                ? Poincare::PrintFloat::SignificantDecimalDigits<float>()
                : Poincare::PrintFloat::SignificantDecimalDigits<double>(),
            m_floatMode);
      }
      EditionReference rack = KRackL()->clone();
      layoutText(rack, buffer);
      AddThousandSeparators(rack);
      NAry::AddOrMergeChild(layoutParent, rack);
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
    case BlockType::Equal:
    case BlockType::NotEqual:
    case BlockType::InferiorEqual:
    case BlockType::Inferior:
    case BlockType::SuperiorEqual:
    case BlockType::Superior:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      addSeparator(layoutParent);
      layoutText(layoutParent, Binary::ComparisonOperatorName(type));
      addSeparator(layoutParent);
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case BlockType::List:
    case BlockType::Set:
    case BlockType::Point:
      if (m_linearMode) {
        PushCodePoint(layoutParent, type.isPoint() ? '(' : '{');
        layoutInfixOperator(layoutParent, expression, ',');
        PushCodePoint(layoutParent, type.isPoint() ? ')' : '}');
        break;
      } else {
        EditionReference parenthesis =
            (type.isList() ? KCurlyBracesL : KParenthesisL)->cloneNode();
        EditionReference rack = KRackL()->clone();
        NAry::AddChild(layoutParent, parenthesis);
        layoutInfixOperator(rack, expression, ',');
        break;
      }
    case BlockType::Parenthesis:
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParenthesis);
      break;
    case BlockType::Store:
    case BlockType::UnitConversion:
      layoutInfixOperator(layoutParent, expression, UCodePointRightwardsArrow);
      break;
#if POINCARE_MEMORY_TREE_LOG
    case BlockType::Placeholder: {
      PushCodePoint(layoutParent, 'K');
      uint8_t offset = Placeholder::NodeToTag(expression);
      Placeholder::Filter filter = Placeholder::NodeToFilter(expression);
      char name = 'A' + offset;
      PushCodePoint(layoutParent, name);
      if (filter != Placeholder::Filter::One) {
        PushCodePoint(layoutParent, '_');
        PushCodePoint(layoutParent,
                      filter == Placeholder::Filter::ZeroOrMore ? 's' : 'p');
      }
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

int FirstNonDigitIndex(Tree *rack) {
  int nonDigitIndex = 0;
  for (const Tree *child : rack->children()) {
    if (!child->isCodePointLayout()) {
      break;
    }
    CodePoint cp = CodePointLayout::GetCodePoint(child);
    if (!((nonDigitIndex == 0 && cp == '-') || ('0' <= cp && cp <= '9'))) {
      break;
    }
    nonDigitIndex++;
  }
  return nonDigitIndex;
}

/* We only display thousands separator if there is more than 4 digits (12 345
 * but 1234) */
constexpr int k_minDigitsForThousandSeparator = 5;
constexpr int k_minValueForThousandSeparator = 10000;

bool Layoutter::AddThousandSeparators(Tree *rack) {
  int nonDigitIndex = FirstNonDigitIndex(rack);
  bool isNegative = rack->child(0)->isCodePointLayout() &&
                    CodePointLayout::GetCodePoint(rack->child(0)) == '-';
  if (nonDigitIndex - isNegative < k_minDigitsForThousandSeparator) {
    return false;
  }
  int index = isNegative + 1;  // skip "-" and first digit
  Tree *digit = rack->child(index);
  while (index < nonDigitIndex) {
    if ((nonDigitIndex - index) % 3 == 0) {
      digit->cloneTreeAtNode(KThousandSeparatorL);
      digit = digit->nextTree();
    }
    digit = digit->nextTree();
    index++;
  }
  NAry::SetNumberOfChildren(
      rack, rack->numberOfChildren() + (nonDigitIndex - isNegative - 1) / 3);
  return true;
}

bool Layoutter::requireSeparators(const Tree *expr) {
  if (expr->isRational()) {
    // TODO PCJ same for decimals and floats
    IntegerHandler num = Rational::Numerator(expr);
    num.setSign(NonStrictSign::Positive);
    if (IntegerHandler::Compare(num, k_minValueForThousandSeparator) >= 0) {
      return true;
    }
    IntegerHandler den = Rational::Denominator(expr);
    if (IntegerHandler::Compare(den, k_minValueForThousandSeparator) >= 0) {
      return true;
    }
    return false;
  }
  if (expr->isFloat() || expr->isDecimal()) {
    /* Since rules are complex with floatDisplayMode, layout the float and check
     * if it has separators. */
    Tree *clone = expr->clone();
    EditionReference rack = KRackL()->clone();
    layoutExpression(rack, clone, k_tokenPriority);
    for (const Tree *child : rack->children()) {
      if (child->isSeparatorLayout()) {
        rack->removeTree();
        return true;
      }
    }
    rack->removeTree();
    return false;
  }
  for (const Tree *child : expr->children()) {
    if (requireSeparators(child)) {
      return true;
    }
    if (expr->isMultiplication() && Units::Unit::IsUnitOrPowerOfUnit(child)) {
      return true;
    }
  }
  return false;
}

void Layoutter::StripSeparators(Tree *rack) {
  assert(rack->isRackLayout());
  Tree *child = rack->nextNode();
  int n = rack->numberOfChildren();
  int i = 0;
  while (i < n) {
    if (child->isSeparatorLayout()) {
      child->removeTree();
      n--;
      continue;
    }
    for (Tree *subRack : child->children()) {
      StripSeparators(subRack);
    }
    child = child->nextTree();
    i++;
  }
  NAry::SetNumberOfChildren(rack, n);
}

bool Layoutter::ImplicitAddition(const Tree *addition) {
  if (addition->numberOfChildren() < 2) {
    return false;
  }
  // Step 1: TODO PCJ check for ᴇ
  // Step 2: Check if units can be implicitly added
  const Units::Representative *storedUnitRepresentative = nullptr;
  for (const Tree *child : addition->children()) {
    if (!child->isMultiplication() || child->numberOfChildren() != 2 ||
        !(child->child(0)->isInteger() ||
          child->child(0)->isOfType({BlockType::Decimal, BlockType::DoubleFloat,
                                     BlockType::SingleFloat})) ||
        !child->child(1)->isUnit()) {
      return false;
    }
    ComplexSign sign = ComplexSign::Get(child->child(0));
    if (!sign.isReal() || !sign.realSign().isPositive()) {
      return false;
    }
    const Units::Representative *childReprensentative =
        Units::Unit::GetRepresentative(child->child(1));
    if (storedUnitRepresentative &&
        !Units::Unit::AllowImplicitAddition(childReprensentative,
                                            storedUnitRepresentative)) {
      return false;
    }
    storedUnitRepresentative = childReprensentative;
  }
  return true;
}

}  // namespace PoincareJ
