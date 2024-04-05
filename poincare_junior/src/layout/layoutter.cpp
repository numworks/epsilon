#include "layoutter.h"

#include <poincare/print_float.h>
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
    case Type::Fact:
    case Type::PercentSimple:
      return 0;
    case Type::Pow:
      return 1;
    case Type::Div:
      return 2;
    case Type::Mult:
      return 3;
    case Type::PercentAddition:
      return 4;
    case Type::Opposite:
    // Opposite could be higher but we prefer to display 2^(-1) instead of 2^-1
    case Type::Sub:
      return 5;
    case Type::Add:
    case Type::MixedFraction:
      return 6;

    case Type::Equal:
    case Type::NotEqual:
    case Type::InferiorEqual:
    case Type::Inferior:
    case Type::SuperiorEqual:
      return 11;

    case Type::LogicalNot:
      return 12;
    case Type::LogicalAnd:
      return 13;
    // TODO PCJ force parentheses on equality
    case Type::LogicalOr:
    case Type::LogicalXor:
      return 14;

    case Type::Point:
    case Type::Set:
    case Type::List:
      return 18;
    case Type::Store:
    case Type::UnitConversion:
      // 2,3→x is read as (2,3)→x not 2,(3→x) (even if invalid)
      return 19;
    case Type::RackLayout:
      return 20;
    default:
      return k_tokenPriority;
  }
}

// Commas have no associated block but behave like an operator
static constexpr int k_commaPriority = OperatorPriority(Type::Set);

Tree* Layoutter::LayoutExpression(Tree* expression, bool linearMode,
                                  int numberOfSignificantDigits,
                                  Preferences::PrintFloatMode floatMode) {
  assert(expression->isExpression());
  /* expression lives before layoutParent in the TreeStack and will be
   * destroyed in the process. An TreeRef is necessary to keep track of
   * layoutParent's root. */
  TreeRef layoutParent = SharedTreeStack->push<Type::RackLayout>(0);
  Layoutter layoutter(linearMode, false, numberOfSignificantDigits, floatMode);
  layoutter.m_addSeparators =
      !linearMode && layoutter.requireSeparators(expression);
  layoutter.layoutExpression(layoutParent, expression, k_maxPriority);
  StripUselessPlus(layoutParent);
  return layoutParent;
}

static void PushCodePoint(Tree* layout, CodePoint codePoint) {
  NAry::AddChild(layout, CodePointLayout::Push(codePoint));
}

static void InsertCodePointAt(Tree* layout, CodePoint codePoint, int index) {
  NAry::AddChildAtIndex(layout, CodePointLayout::Push(codePoint), index);
}

void Layoutter::addSeparator(Tree* layoutParent) {
  if (!m_addSeparators) {
    return;
  }
  assert(!m_linearMode);
  NAry::AddChild(layoutParent, KOperatorSeparatorL->clone());
}

void Layoutter::layoutText(TreeRef& layoutParent, const char* text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    PushCodePoint(layoutParent, codePoint);
    codePoint = decoder.nextCodePoint();
  }
}

void Layoutter::layoutBuiltin(TreeRef& layoutParent, Tree* expression) {
  assert(Builtin::IsReservedFunction(expression));
  const Builtin* builtin = Builtin::GetReservedFunction(expression);
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
    const BuiltinWithLayout* builtinWithLayout =
        static_cast<const BuiltinWithLayout*>(builtin);
    TreeRef layout = SharedTreeStack->push(
        static_cast<Type>(builtinWithLayout->layoutType()));
    /* Some builtins have a bigger nodeSize. Additional parameters are not
     * handled here. TODO_PCJ: Remove this one these Layouts are moved out of
     * builtins. */
    for (size_t i = 1; i < layout->nodeSize(); i++) {
      SharedTreeStack->push(Type::Zero);
    }
    layoutChildrenAsRacks(expression);
    NAry::AddChild(layoutParent, layout);
  }
}

void Layoutter::layoutFunctionCall(TreeRef& layoutParent, Tree* expression,
                                   const char* name) {
  layoutText(layoutParent, name);
  TreeRef parenthesis =
      SharedTreeStack->push<Type::ParenthesisLayout>(false, false);
  TreeRef newParent = SharedTreeStack->push<Type::RackLayout>(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (j == 1 && expression->isListStatWithCoefficients() &&
        expression->nextNode()->isOne()) {
      // Skip coefficient if default mean(L, 1) -> mean(L)
      expression->nextNode()->removeTree();
      continue;
    }
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    layoutExpression(newParent, expression->nextNode(), k_commaPriority);
  }
}

void Layoutter::layoutChildrenAsRacks(Tree* expression) {
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    TreeRef newParent = SharedTreeStack->push<Type::RackLayout>(0);
    layoutExpression(newParent, expression->nextNode(), k_maxPriority);
  }
}

void Layoutter::layoutIntegerHandler(TreeRef& layoutParent,
                                     IntegerHandler handler,
                                     int decimalOffset) {
  if (handler.strictSign() == StrictSign::Negative) {
    PushCodePoint(layoutParent, '-');
  }
  handler.setSign(NonStrictSign::Positive);
  Tree* rack = KRackL()->clone();
  /* We can't manipulate an IntegerHandler in a workingBuffer since we're
   * pushing layouts on the TreeStack at each steps. Value is therefore
   * temporarily stored and updated on the TreeStack. */
  TreeRef value = handler.pushOnTreeStack();
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

void Layoutter::layoutInfixOperator(TreeRef& layoutParent, Tree* expression,
                                    CodePoint op, bool multiplication) {
  Type type = expression->type();
  int childNumber = expression->numberOfChildren();
  bool previousWasUnit = false;
  for (int childIndex = 0; childIndex < childNumber; childIndex++) {
    Tree* child = expression->nextNode();
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

void Layoutter::layoutMatrix(TreeRef& layoutParent, Tree* expression) {
  if (!m_linearMode) {
    TreeRef layout = expression->cloneNode();
    *layout->block() = Type::MatrixLayout;
    layoutChildrenAsRacks(expression);
    NAry::AddChild(layoutParent, layout);
    Grid* grid = Grid::From(layout);
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

void Layoutter::layoutUnit(TreeRef& layoutParent, Tree* expression) {
  // TODO PCJ ask the context whether to add an underscore
  if (m_linearMode) {
    PushCodePoint(layoutParent, '_');
  }
  layoutText(layoutParent, Units::Unit::GetPrefix(expression)->symbol());
  layoutText(
      layoutParent,
      Units::Unit::GetRepresentative(expression)->rootSymbols().mainAlias());
}

void Layoutter::layoutPowerOrDivision(TreeRef& layoutParent, Tree* expression) {
  Type type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->nextNode();
  TreeRef createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (m_linearMode) {
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    PushCodePoint(layoutParent, type == Type::Div ? '/' : '^');
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    return;
  }
  if (type == Type::Div) {
    createdLayout = SharedTreeStack->push(Type::FractionLayout);
    TreeRef rack = SharedTreeStack->push<Type::RackLayout>(0);
    layoutExpression(rack, expression, k_maxPriority);
  } else {
    assert(type == Type::Pow || type == Type::PowMatrix);
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    createdLayout = KSuperscriptL->cloneNode();
  }
  TreeRef rack = SharedTreeStack->push<Type::RackLayout>(0);
  layoutExpression(rack, expression, k_maxPriority);
  NAry::AddChild(layoutParent, createdLayout);
}

// Remove expression while converting it to a layout in layoutParent
void Layoutter::layoutExpression(TreeRef& layoutParent, Tree* expression,
                                 int parentPriority) {
  assert(layoutParent->isRackLayout());
  TypeBlock type = expression->type();

  // Add Parentheses if necessary
  if (parentPriority < OperatorPriority(type) &&
      !(type.isPoint() || type.isList())) {
    TreeRef parenthesis = KParenthesisL(KRackL())->clone();
    NAry::AddChild(layoutParent, parenthesis);
    TreeRef rack = parenthesis->child(0);
    return layoutExpression(rack, expression, k_maxPriority);
  }

  switch (type) {
    case Type::Add: {
      CodePoint op =
          ImplicitAddition(expression) ? UCodePointNull : CodePoint('+');
      layoutInfixOperator(layoutParent, expression, op);
      break;
    }
    case Type::Mult:
      /* TODO PCJ: Add small margins when units are present */
      layoutInfixOperator(
          layoutParent, expression,
          m_linearMode ? CodePoint(u'×') : MultiplicationSymbol(expression),
          true);
      break;
    case Type::Pow:
    case Type::PowMatrix:
    case Type::Div:
      layoutPowerOrDivision(layoutParent, expression);
      break;
    case Type::Sub:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(Type::Add));
      addSeparator(layoutParent);
      PushCodePoint(layoutParent, '-');
      addSeparator(layoutParent);
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case Type::Opposite: {
      PushCodePoint(layoutParent, '-');
      // Add extra parentheses for -1^2 -> -(1^2) but not for -x^2
      bool addExtraParenthesis =
          expression->child(0)->isPow() &&
          !expression->child(0)->child(0)->isUserSymbol();
      layoutExpression(
          layoutParent, expression->nextNode(),
          addExtraParenthesis ? k_forceParenthesis : OperatorPriority(type));
      break;
    }
    case Type::Fact:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '!');
      break;
    case Type::PercentAddition:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      if (expression->nextNode()->isOpposite()) {
        PushCodePoint(layoutParent, UCodePointSouthEastArrow);
        expression->nextNode()->removeNode();
      } else {
        PushCodePoint(layoutParent, UCodePointNorthEastArrow);
      }
      // continue
    case Type::PercentSimple:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      PushCodePoint(layoutParent, '%');
      break;
    case Type::Zero:
    case Type::MinusOne:
    case Type::One:
    case Type::Two:
    case Type::IntegerShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
      // TODO PCJ we need a way to layout an integer in base something
      layoutIntegerHandler(layoutParent, Integer::Handler(expression));
      break;
    case Type::Half:
    case Type::RationalShort:
    case Type::RationalPosBig:
    case Type::RationalNegBig: {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
      layoutIntegerHandler(layoutParent, Rational::Numerator(expression));
      PushCodePoint(layoutParent, '/');
      layoutIntegerHandler(layoutParent, Rational::Denominator(expression));
#else
      // Expression should have been beautified before layoutting
      assert(false);
#endif
      break;
    }
    case Type::Diff:
    case Type::NthDiff:
      // TODO_PCJ createValidExpandedForm
      if (expression->lastChild()->isUserFunction() &&
          expression->lastChild()->child(0)->isUserSymbol() &&
          expression->lastChild()->child(0)->treeIsIdenticalTo(
              Symbol::k_systemSymbol)) {
        layoutText(layoutParent, Symbol::GetName(expression->lastChild()));
        int order = expression->isDiff()
                        ? 1
                        : Integer::Handler(expression->child(2)).to<int>();
        if (order <= 2) {
          PushCodePoint(layoutParent, order == 1 ? '\'' : '"');
          if (expression->isNthDiff()) {
            expression->child(2)->removeTree();
          }
        } else {
          assert(expression->isNthDiff());
          TreeRef rack;
          if (m_linearMode) {
            PushCodePoint(layoutParent, '^');
            rack = layoutParent;
          } else {
            Tree* createdLayout = KSuperscriptL->cloneNode();
            rack = SharedTreeStack->push<Type::RackLayout>(0);
            NAry::AddChild(layoutParent, createdLayout);
          }
          layoutExpression(rack, expression->child(2), k_forceParenthesis);
        }
        expression->child(2)->removeTree();
        expression->child(0)->removeTree();
        layoutExpression(layoutParent, expression->nextNode(),
                         k_forceParenthesis);
        break;
      }
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        TreeRef layout =
            (type.isDiff() ? KDerivativeL : KNthDerivativeL)->cloneNode();
        if (type.isNthDiff()) {
          // Handle the peculiar order of nth-derivative layout
          // TODO fix order in derivative layout instead
          expression->child(2)->moveTreeBeforeNode(expression->child(3));
        }
        layoutChildrenAsRacks(expression);
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case Type::Binomial:
    case Type::Permute:
      if (m_linearMode ||
          Preferences::SharedPreferences()->combinatoricSymbols() ==
              Preferences::CombinatoricSymbols::Default) {
        layoutBuiltin(layoutParent, expression);
      } else {
        TreeRef layout = SharedTreeStack->push(
            type.isBinomial() ? Type::PtBinomialLayout : Type::PtPermuteLayout);
        layoutChildrenAsRacks(expression);
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case Type::Logarithm: {
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
        break;
      }
      constexpr const char* log =
          Builtin::GetReservedFunction(Type::Logarithm)->aliases()->mainAlias();
      bool nlLog = Preferences::SharedPreferences()->logarithmBasePosition() ==
                   Preferences::LogarithmBasePosition::TopLeft;
      if (!nlLog) {
        layoutText(layoutParent, log);
      }
      // Base
      TreeRef layout =
          nlLog ? KPrefixSuperscriptL->cloneNode() : KSubscriptL->cloneNode();
      TreeRef newParent = KRackL()->clone();
      layoutExpression(newParent, expression->child(1), k_maxPriority);
      NAry::AddChild(layoutParent, layout);
      if (nlLog) {
        layoutText(layoutParent, log);
      }
      // Value
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParenthesis);
      break;
    }
    case Type::MixedFraction:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      if (m_linearMode) {
        // TODO PCJ make sure the serializer makes the distinction too
        PushCodePoint(layoutParent, ' ');
      }
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case Type::Pi:
      PushCodePoint(layoutParent, u'π');
      break;
    case Type::EulerE:
      PushCodePoint(layoutParent, 'e');
      break;
    case Type::ComplexI:
      PushCodePoint(layoutParent, 'i');
      break;
    case Type::PhysicalConstant:
      layoutText(layoutParent,
                 Constant::Info(expression).m_aliasesList.mainAlias());
      break;
    case Type::UserSymbol:
    case Type::UserSequence:
    case Type::UserFunction: {
      layoutText(layoutParent, Symbol::GetName(expression));
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
          TreeRef layout = KSubscriptL->cloneNode();
          layoutChildrenAsRacks(expression);
          NAry::AddChild(layoutParent, layout);
        }
      }
      break;
    }
    case Type::Inf:
    case Type::NonReal:
    case Type::Undef:
      layoutText(layoutParent,
                 Builtin::SpecialIdentifierName(type).mainAlias());
      break;
    case Type::Empty:
      break;
    case Type::Matrix:
      layoutMatrix(layoutParent, expression);
      break;
    case Type::Piecewise:
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        int rows = (expression->numberOfChildren() + 1) / 2;
        TreeRef layout = SharedTreeStack->push(Type::PiecewiseLayout);
        SharedTreeStack->push(rows + 1);
        SharedTreeStack->push(2);
        layoutChildrenAsRacks(expression);
        // Placeholders
        if (expression->numberOfChildren() % 2 == 1) {
          KRackL()->clone();
        }
        KRackL()->clone();
        KRackL()->clone();
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case Type::Unit:
      layoutUnit(layoutParent, expression);
      break;
    case Type::Decimal:
    case Type::SingleFloat:
    case Type::DoubleFloat: {
      char buffer[100];
      if (type.isDecimal()) {
        Decimal::Serialize(expression, buffer, std::size(buffer), m_floatMode,
                           m_numberOfSignificantDigits);
        expression->nextNode()->removeTree();
      } else {
        Poincare::PrintFloat::ConvertFloatToText(
            FloatNode::To(expression), buffer, std::size(buffer),
            Poincare::PrintFloat::k_maxFloatGlyphLength,
            m_numberOfSignificantDigits != -1 ? m_numberOfSignificantDigits
            : type == Type::SingleFloat
                ? Poincare::PrintFloat::SignificantDecimalDigits<float>()
                : Poincare::PrintFloat::SignificantDecimalDigits<double>(),
            m_floatMode);
      }
      TreeRef rack = KRackL()->clone();
      layoutText(rack, buffer);
      AddThousandSeparators(rack);
      NAry::AddOrMergeChild(layoutParent, rack);
      break;
    }
    case Type::True:
      layoutText(layoutParent, BuiltinsAliases::k_trueAliases.mainAlias());
      break;
    case Type::False:
      layoutText(layoutParent, BuiltinsAliases::k_falseAliases.mainAlias());
      break;
    case Type::LogicalAnd:
    case Type::LogicalOr:
    case Type::LogicalXor:
    case Type::LogicalNot:
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
    case Type::Equal:
    case Type::NotEqual:
    case Type::InferiorEqual:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::Superior:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      addSeparator(layoutParent);
      layoutText(layoutParent, Binary::ComparisonOperatorName(type));
      addSeparator(layoutParent);
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case Type::List:
    case Type::Set:
    case Type::Point:
      if (m_linearMode) {
        PushCodePoint(layoutParent, type.isPoint() ? '(' : '{');
        layoutInfixOperator(layoutParent, expression, ',');
        PushCodePoint(layoutParent, type.isPoint() ? ')' : '}');
        break;
      } else {
        TreeRef parenthesis =
            (type.isList() ? KCurlyBracesL : KParenthesisL)->cloneNode();
        TreeRef rack = KRackL()->clone();
        NAry::AddChild(layoutParent, parenthesis);
        layoutInfixOperator(rack, expression, ',');
        break;
      }
    case Type::Parenthesis:
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParenthesis);
      break;
    case Type::Store:
    case Type::UnitConversion:
      layoutInfixOperator(layoutParent, expression, UCodePointRightwardsArrow);
      break;
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
    case Type::Placeholder: {
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
    case Type::Exp:
      layoutFunctionCall(layoutParent, expression, "exp");
      break;
    case Type::Var: {
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
    case Type::Polynomial:
    default:
      if (Builtin::IsReservedFunction(expression)) {
        layoutBuiltin(layoutParent, expression);
      } else {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
        layoutFunctionCall(
            layoutParent, expression,
            TypeBlock::names[static_cast<uint8_t>(*expression->block())]);
#else
        // TODO: Handle missing Types
        assert(false);
        PushCodePoint(layoutParent, '?');
#endif
        break;
      }
  }
  // Children have already been removed.
  expression->removeNode();
}

int FirstNonDigitIndex(Tree* rack) {
  int nonDigitIndex = 0;
  for (const Tree* child : rack->children()) {
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

bool Layoutter::AddThousandSeparators(Tree* rack) {
  int nonDigitIndex = FirstNonDigitIndex(rack);
  bool isNegative = rack->child(0)->isCodePointLayout() &&
                    CodePointLayout::GetCodePoint(rack->child(0)) == '-';
  if (nonDigitIndex - isNegative < k_minDigitsForThousandSeparator) {
    return false;
  }
  int index = isNegative + 1;  // skip "-" and first digit
  Tree* digit = rack->child(index);
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

bool Layoutter::requireSeparators(const Tree* expr) {
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
    Tree* clone = expr->clone();
    TreeRef rack = KRackL()->clone();
    layoutExpression(rack, clone, k_tokenPriority);
    for (const Tree* child : rack->children()) {
      if (child->isSeparatorLayout()) {
        rack->removeTree();
        return true;
      }
    }
    rack->removeTree();
    return false;
  }
  for (const Tree* child : expr->children()) {
    if (requireSeparators(child)) {
      return true;
    }
    if (expr->isMult() && Units::Unit::IsUnitOrPowerOfUnit(child)) {
      return true;
    }
  }
  return false;
}

void Layoutter::StripSeparators(Tree* rack) {
  assert(rack->isRackLayout());
  Tree* child = rack->nextNode();
  int n = rack->numberOfChildren();
  int i = 0;
  while (i < n) {
    if (child->isSeparatorLayout()) {
      child->removeTree();
      n--;
      continue;
    }
    for (Tree* subRack : child->children()) {
      StripSeparators(subRack);
    }
    child = child->nextTree();
    i++;
  }
  NAry::SetNumberOfChildren(rack, n);
}

void Layoutter::StripUselessPlus(Tree* rack) {
  assert(rack->isRackLayout());
  Tree* child = rack->nextNode();
  int n = rack->numberOfChildren();
  int i = 0;
  Tree* previousPlus = nullptr;
  while (i < n) {
    if (child->isCodePointLayout()) {
      if (previousPlus && CodePointLayout::GetCodePoint(child) == '-') {
        assert(previousPlus->nextTree() == child &&
               previousPlus->treeSize() == child->treeSize());
        previousPlus->removeTree();
        previousPlus = nullptr;
        n--;
        continue;
      }
      previousPlus =
          CodePointLayout::GetCodePoint(child) == '+' ? child : nullptr;
    } else {
      previousPlus = nullptr;
    }
    for (Tree* subRack : child->children()) {
      StripUselessPlus(subRack);
    }
    child = child->nextTree();
    i++;
  }
  NAry::SetNumberOfChildren(rack, n);
}

bool Layoutter::ImplicitAddition(const Tree* addition) {
  if (addition->numberOfChildren() < 2) {
    return false;
  }
  // Step 1: TODO PCJ check that no ᴇ will be needed
  // Step 2: Check if units can be implicitly added
  const Units::Representative* storedUnitRepresentative = nullptr;
  for (const Tree* child : addition->children()) {
    if (!(child->isMult() && child->numberOfChildren() == 2 &&
          (child->child(0)->isInteger() ||
           child->child(0)->isOfType(
               {Type::Decimal, Type::DoubleFloat, Type::SingleFloat})) &&
          child->child(1)->isUnit())) {
      return false;
    }
    ComplexSign sign = ComplexSign::Get(child->child(0));
    if (!sign.isReal() || !sign.realSign().isPositive()) {
      return false;
    }
    const Units::Representative* childReprensentative =
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
