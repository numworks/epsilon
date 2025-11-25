#include "layouter.h"

#include <omg/string.h>
#include <omg/unreachable.h>
#include <omg/utf8_helper.h>
#include <poincare/comparison_operator.h>
#include <poincare/helpers/layout.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/decimal.h>
#include <poincare/src/expression/derivation.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/number.h>
#include <poincare/src/expression/physical_constant.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sequence.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/placeholder.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/symbol_context.h>

#include "grid.h"
#include "k_tree.h"
#include "multiplication_symbol.h"

namespace Poincare::Internal {

constexpr static int k_forceParentheses = -2;

// A single token will never need parentheses
constexpr static int k_tokenPriority = -1;

// MaxPriority is to be used when there is no parent that could cause confusion
constexpr static int k_maxPriority = 20;

constexpr static int k_forceRemoveParentheses = k_maxPriority;

/* Priority just after Add for left child of a subtraction that may be an
 * unparenthesed addition */
constexpr static int k_subLeftChildPriority = 9;

constexpr static int OperatorPriority(TypeBlock type) {
  switch (type) {
    case Type::ListElement:
    case Type::ListSlice:
      return 0;
    case Type::Fact:
    case Type::PercentSimple:
      return 1;
    case Type::Pow:
      return 2;
    case Type::Div:
      return 3;
    case Type::EuclideanDivision:
      return 4;
    case Type::Mult:
      return 5;
    case Type::PercentAddition:
    case Type::MixedFraction:
      return 6;
    case Type::Opposite:
    // Opposite could be higher but we prefer to display 2^(-1) instead of 2^-1
    case Type::Sub:
      return 7;
    case Type::Add:
      return 8;
      static_assert(k_subLeftChildPriority == 9);

    case Type::Equal:
    case Type::NotEqual:
    case Type::InferiorEqual:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::Superior:
      return 11;

    case Type::LogicalNot:
      return 12;
    case Type::LogicalAnd:
    case Type::LogicalNand:
      return 13;
    case Type::LogicalOr:
    case Type::LogicalNor:
    case Type::LogicalXor:
      return 14;

    case Type::Point:
    case Type::DepList:
    case Type::List:
      return 18;
    case Type::Store:
    case Type::UnitConversion:
      // 2,3→x is read as (2,3)→x not 2,(3→x) (even if invalid)
      return 19;
    case Type::RackSimpleLayout:
      return 20;
    default:
      return k_tokenPriority;
  }
}

// Commas have no associated block but behave like an operator
constexpr static int k_commaPriority = OperatorPriority(Type::List);

Tree* Layouter::LayoutExpression(const Tree* expression,
                                 LayouterParameters params) {
  ExceptionTry { return UnsafeLayoutExpression(expression, params); }
  ExceptionCatch(exc) {
    switch (exc) {
      case ExceptionType::TreeStackOverflow:
      case ExceptionType::IntegerOverflow:
        return UnsafeLayoutExpression(KUndef, params);
      default:
        TreeStackCheckpoint::Raise(exc);
    }
  }
  OMG::unreachable();
}

Tree* Layouter::UnsafeLayoutExpression(const Tree* expression,
                                       LayouterParameters params) {
  assert(expression->isExpression() || expression->isPlaceholder());
  TreeRef rack = SharedTreeStack->pushRackLayout(0);
  Layouter layouter(params);
  layouter.m_addSeparators = params.layouterMode == LayouterMode::Natural &&
                             layouter.requireSeparators(expression);
  layouter.layoutExpression(rack, expression, k_maxPriority);
  StripUselessPlus(rack);
  if (params.useTenPowerInsteadOfE) {
    Poincare::LayoutHelpers::TurnEToTenPowerLayout(rack, layouter.linearMode());
  }
  return rack;
}

static void PushCodePoint(Tree* rack, CodePoint codePoint) {
  NAry::AddChild(rack, CodePointLayout::Push(codePoint));
}

static void PushCombinedCodePoint(Tree* rack, CodePoint codePoint,
                                  CodePoint combiningCodePoint) {
  NAry::AddChild(rack,
                 CodePointLayout::PushCombined(codePoint, combiningCodePoint));
}

static void InsertCodePointAt(Tree* rack, CodePoint codePoint, int index) {
  NAry::AddChildAtIndex(rack, CodePointLayout::Push(codePoint), index);
}

void Layouter::addOperatorSeparator(Tree* parentRack) {
  if (!m_addSeparators) {
    return;
  }
  assert(!linearMode());
  NAry::AddChild(parentRack, KOperatorSeparatorL->cloneTree());
}

void Layouter::addUnitSeparator(Tree* parentRack) {
  if (!m_addSeparators) {
    return;
  }
  assert(!linearMode());
  NAry::AddChild(parentRack, KUnitSeparatorL->cloneTree());
}

Tree* Layouter::insertParenthesis(Tree* parentRack, bool isOpening,
                                  bool isCurlyBrace) {
  if (linearMode()) {
    PushCodePoint(parentRack, isOpening      ? isCurlyBrace ? '{' : '('
                              : isCurlyBrace ? '}'
                                             : ')');
    return parentRack;
  } else if (isOpening) {
    Tree* parenthesis =
        (isCurlyBrace ? KCurlyBracesL : KParenthesesL)->cloneNode();
    KRackL()->cloneTree();
    NAry::AddChild(parentRack, parenthesis);
    return parenthesis->child(0);
  }
  // If closing layout parentheses, do nothing.
  return parentRack;
}

void Layouter::layoutText(Tree* parentRack, std::string_view text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    CodePoint nextCodePoint = decoder.nextCodePoint();
    if (nextCodePoint.isCombining()) {
      PushCombinedCodePoint(parentRack, codePoint, nextCodePoint);
      codePoint = decoder.nextCodePoint();
    } else {
      PushCodePoint(parentRack, codePoint);
      codePoint = nextCodePoint;
    }
  }
}

void Layouter::layoutBuiltin(Tree* parentRack, const Tree* expression) {
  assert(Builtin::IsReservedFunction(expression));
  const Builtin* builtin = Builtin::GetReservedFunction(
      expression, SharedPreferences->translateBuiltins());
  if (linearMode() || !builtin->has2DLayout()) {
    // Built "builtin(child1, child2)"
    const char* name = builtin->aliases()->mainAlias();
    layoutFunctionCall(parentRack, expression, name);
  } else {
    // Built 2D layout associated with builtin
    const BuiltinWithLayout* builtinWithLayout =
        static_cast<const BuiltinWithLayout*>(builtin);
    Tree* layout = SharedTreeStack->pushBlock(builtinWithLayout->layoutType());
    layoutChildrenAsRacks(expression);
    NAry::AddChild(parentRack, layout);
  }
}

void Layouter::layoutFunctionCall(Tree* parentRack, const Tree* expression,
                                  std::string_view name) {
  layoutText(parentRack, name);
  Tree* newParent = insertParenthesis(parentRack, true);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    int childIndex = j;
    if (expression->isParametric()) {
      if (j == 0) {
        // The first item to display is the last child (the integrand)
        childIndex = expression->numberOfChildren() - 1;
      } else {
        childIndex--;
      }
    }
    if (((childIndex == 1 && expression->isListStatWithCoefficients()) ||
         (childIndex == 2 && expression->isDiff())) &&
        expression->child(childIndex)->isOne()) {
      // TODO: factorize with PromoteBuiltin?
      // TODO: factorize with 2D layouting?
      /* Remove default parameters:
       * - mean(L, 1) -> mean(L)
       * - diff(f, x, y, 1) -> diff(f, x, y) */
      continue;
    }
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    layoutExpression(newParent, expression->child(childIndex), k_commaPriority);
  }
  insertParenthesis(newParent, false);
}

void Layouter::layoutChildrenAsRacks(const Tree* expression) {
  for (const Tree* child : expression->children()) {
    Tree* newParent = SharedTreeStack->pushRackLayout(0);
    layoutExpression(newParent, child, k_maxPriority);
  }
}

void Layouter::layoutIntegerHandler(Tree* parentRack, IntegerHandler handler,
                                    int decimalOffset) {
  if (handler.strictSign() == StrictSign::Negative) {
    PushCodePoint(parentRack, '-');
  }
  handler.setSign(NonStrictSign::Positive);
  if (m_parameters.base != OMG::Base::Decimal) {
    assert(decimalOffset == 0);
    PushCodePoint(parentRack, '0');
    PushCodePoint(parentRack,
                  m_parameters.base == OMG::Base::Binary ? 'b' : 'x');
  }
  Tree* rack = KRackL()->cloneTree();
  /* We can't manipulate an IntegerHandler in a workingBuffer since we're
   * pushing layouts on the TreeStack at each steps. Value is therefore
   * temporarily stored and updated on the TreeStack. */
  TreeRef value = handler.pushOnTreeStack();
  do {
    DivisionResult result = IntegerHandler::Division(
        Integer::Handler(value),
        IntegerHandler(static_cast<uint8_t>(m_parameters.base)));
    uint8_t digit = Integer::Handler(result.remainder);
    assert(result.remainder > result.quotient);
    result.remainder->removeTree();
    value->moveTreeOverTree(result.quotient);
    // Map 0 1 2 ... 9 10 11 ... 15 to char '0' '1' '2' ... '9' 'A' 'B' ... 'F'
    assert(digit < 16);
    char codePoint = (digit < 10 ? '0' : 'A' - 10) + digit;
    InsertCodePointAt(rack, codePoint, 0);
    if (--decimalOffset == 0) {
      InsertCodePointAt(rack, '.', 0);
      if (value->isZero()) {
        // TODO_PCJ: insert 0 before . if nothing else is to come
        InsertCodePointAt(rack, '0', 0);
      }
    }
  } while (!(value->isZero() && decimalOffset <= 0));
  value->removeTree();
  if (m_parameters.base == OMG::Base::Decimal && !linearMode()) {
    AddThousandsSeparators(rack);
  }
  NAry::AddOrMergeChild(parentRack, rack);
}

void Layouter::layoutInfixOperator(Tree* parentRack, const Tree* expression,
                                   CodePoint op, bool multiplication) {
  Type type = expression->type();
  int operatorPriority = OperatorPriority(type);
  bool previousWasUnit = false;
  for (IndexedChild<const Tree*> child : expression->indexedChildren()) {
    bool isUnit = Units::Unit::IsUnitOrPowerOfUnit(child);
    const Tree* targetChild = child;
    if (child.index > 0) {
      if (!linearMode() && multiplication && isUnit) {
        if (!previousWasUnit) {
          if (Units::Unit::ForceMarginLeftOfUnit(child)) {
            // Add small separator between 2 and m in "2 m"
            addUnitSeparator(parentRack);
          }
        } else {
          PushCodePoint(parentRack, UCodePointMiddleDot);
        }
        layoutExpression(parentRack, child, operatorPriority);
        previousWasUnit = isUnit;
        continue;
      }
      addOperatorSeparator(parentRack);
      if (op != UCodePointNull) {
        if (op == '+' && child->isOpposite()) {
          // Consume opposite block now and insert - instead of +
          PushCodePoint(parentRack, '-');
          targetChild = targetChild->child(0);
          if (OperatorPriority(Type::Opposite) <
              OperatorPriority(child->type())) {
            // Add(A, Oppose(Add(A, B))) -> A - ( B + C )
            operatorPriority = k_forceParentheses;
          }
        } else {
          PushCodePoint(parentRack, op);
        }
        addOperatorSeparator(parentRack);
      }
    }
    layoutExpression(parentRack, targetChild, operatorPriority);
    previousWasUnit = isUnit;
  }
}

void Layouter::layoutMatrix(Tree* parentRack, const Tree* expression) {
  if (!linearMode()) {
    Tree* layout = expression->cloneNode();
    *layout->block() = Type::MatrixLayout;
    layoutChildrenAsRacks(expression);
    NAry::AddChild(parentRack, layout);
    Grid* grid = Grid::From(layout);
    grid->addEmptyColumn();
    grid->addEmptyRow();
    return;
  }
  PushCodePoint(parentRack, '[');
  int cols = Matrix::NumberOfColumns(expression);
  int rows = Matrix::NumberOfRows(expression);
  const Tree* child = expression->nextNode();
  for (int row = 0; row < rows; row++) {
    PushCodePoint(parentRack, '[');
    for (int col = 0; col < cols; col++) {
      if (col > 0) {
        PushCodePoint(parentRack, ',');
      }
      layoutExpression(parentRack, child, k_commaPriority);
      child = child->nextTree();
    }
    PushCodePoint(parentRack, ']');
  }
  PushCodePoint(parentRack, ']');
}

void Layouter::layoutSequence(Tree* parentRack, const Tree* expression) {
#if POINCARE_SEQUENCE
  // Get first rank
  assert(Integer::Handler(expression->child(Sequence::k_firstRankIndex))
             .is<uint8_t>());
  uint8_t firstRank =
      Integer::Handler(expression->child(Sequence::k_firstRankIndex))
          .to<uint8_t>();

  // Get names
  TreeRef mainExpressionName = expression->child(0)->cloneTree();
  TreeRef firstInitialConditionName =
      (expression->type() == Type::SequenceSingleRecurrence ||
       expression->type() == Type::SequenceDoubleRecurrence)
          ? Sequence::PushInitialConditionName(expression, true)
          : nullptr;
  TreeRef secondInitialConditionName =
      (expression->type() == Type::SequenceDoubleRecurrence)
          ? Sequence::PushInitialConditionName(expression, false)
          : nullptr;

  // Push sequence layout
  assert(expression->numberOfChildren() >= 2 &&
         expression->numberOfChildren() <= 5);
  uint8_t numberOfExpressionChildren = expression->numberOfChildren() - 2;
  TreeRef layout = SharedTreeStack->pushSequenceLayout(
      numberOfExpressionChildren, 2, firstRank);

  // Skip name
  const Tree* currentChild = expression->child(1);

  // Layout main expression and initial conditions
  for (int i = 0; i < numberOfExpressionChildren; i++) {
    Tree* name = i == 0   ? mainExpressionName
                 : i == 1 ? firstInitialConditionName
                          : secondInitialConditionName;
    assert(name);
    Tree* newParent = SharedTreeStack->pushRackLayout(0);
    layoutExpression(newParent, name, k_maxPriority);
    name->removeTree();
    newParent = SharedTreeStack->pushRackLayout(0);
    layoutExpression(newParent, currentChild, k_maxPriority);
    currentChild = currentChild->nextTree();
    if (i == 0) {
      // Skip first rank
      currentChild = currentChild->nextTree();
    }
  }
  NAry::AddChild(parentRack, layout);
#endif
}

void Layouter::layoutUnit(Tree* parentRack, const Tree* expression) {
#if POINCARE_UNIT
  using namespace Units;
  OMG::String<Prefix::k_maxTextLen> prefixText(
      Unit::GetPrefix(expression)->symbol());
  const Representative* representative = Unit::GetRepresentative(expression);
  OMG::String<Representative::k_maxTextLen> representativeText(
      representative->rootSymbols().mainAlias());
  OMG::String<Unit::k_maxTextLen> unitText = prefixText + representativeText;

  if (linearMode() || (!Unit::IsNameReserved(representative) &&
                       (m_parameters.symbolContext.useStrictUnitLayout() ||
                        m_parameters.symbolContext.expressionTypeForIdentifier(
                            unitText) != SymbolContext::UserNamedType::None))) {
    PushCodePoint(parentRack, '_');
  }

  layoutText(parentRack, unitText);
#endif
}

void Layouter::layoutPowerOrDivision(Tree* parentRack, const Tree* expression) {
  Tree* newLayout;
  const Tree* firstChild = expression->child(0);
  const Tree* secondChild = firstChild->nextTree();
  // No parentheses in Fraction roots and Power index.
  if (linearMode()) {
    // In compact mode, parentheses are removed from (a×b)/c
    int firstChildPriority =
        (expression->isDiv() && firstChild->isMult() &&
         m_parameters.layouterMode == LayouterMode::LinearCompact)
            ? k_forceRemoveParentheses
            : OperatorPriority(expression->type());
    // force parentheses when serializing e^(x)
    int secondChildPriority = expression->isPow() && firstChild->isEulerE()
                                  ? k_forceParentheses
                                  : OperatorPriority(expression->type());
    layoutExpression(parentRack, firstChild, firstChildPriority);
    PushCodePoint(parentRack, expression->isDiv() ? '/' : '^');
    layoutExpression(parentRack, secondChild, secondChildPriority);
    return;
  }
  if (expression->isDiv()) {
    newLayout = SharedTreeStack->pushFractionLayout();
    TreeRef rack = SharedTreeStack->pushRackLayout(0);
    layoutExpression(rack, firstChild, k_maxPriority);
  } else {
    assert(expression->isPow());
    layoutExpression(parentRack, firstChild,
                     OperatorPriority(expression->type()));
    newLayout = KSuperscriptL->cloneNode();
  }
  TreeRef rack = SharedTreeStack->pushRackLayout(0);
  layoutExpression(rack, secondChild, k_maxPriority);
  NAry::AddChild(parentRack, newLayout);
}

void Layouter::serializeDecimalOrFloat(const Tree* expression, char* buffer,
                                       size_t bufferSize) {
  assert(expression->isOfType(
      {Type::Decimal, Type::DoubleFloat, Type::SingleFloat}));
  if (expression->isDecimal()) {
    /* Just as it doesn't apply to integers, numberOfSignificantDigits is
     * ignored with Decimals. */
    Decimal::Serialize(expression, buffer, bufferSize, m_parameters.floatMode);
    return;
  }
  int numberOfSignificantDigits =
      m_parameters.numberOfSignificantDigits !=
              PrintFloat::k_undefinedNumberOfSignificantDigits
          ? m_parameters.numberOfSignificantDigits
      : expression->isSingleFloat()
          ? Poincare::PrintFloat::SignificantDecimalDigits<float>()
          : Poincare::PrintFloat::SignificantDecimalDigits<double>();
  Poincare::PrintFloat::ConvertFloatToText(
      FloatHelper::To(expression), buffer, bufferSize,
      Poincare::PrintFloat::k_maxFloatGlyphLength, numberOfSignificantDigits,
      m_parameters.floatMode);
}

// Remove expression while converting it to a layout in parentRack
void Layouter::layoutExpression(Tree* parentRack, const Tree* expression,
                                int parentPriority) {
  assert(parentRack->isRackLayout());
  TypeBlock type = expression->type();

  // Add Parentheses if necessary
  if ((parentPriority <= OperatorPriority(type) &&
       (!(type.isPoint() || type.isDepList() || type.isList()) ||
        parentPriority == k_forceParentheses)) ||
      (type.isEuclideanDivision() &&
       parentPriority < OperatorPriority(Type::Equal))) {
    Tree* newParent = insertParenthesis(parentRack, true);
    layoutExpression(newParent, expression, k_maxPriority);
    insertParenthesis(newParent, false);
    return;
  }
  // Point to the next child to be laid out
  const Tree* child = expression->nextNode();
  switch (type) {
    // TODO_PCJ: Restore Addition and Multiplication symbol in linear mode
    case Type::Add: {
      CodePoint op = implicitAddition(expression) && !linearMode()
                         ? UCodePointNull
                         : CodePoint('+');
      layoutInfixOperator(parentRack, expression, op);
      break;
    }
    case Type::Mult:
      layoutInfixOperator(parentRack, expression,
                          (m_parameters.layouterMode == LayouterMode::Linear)
                              ? CodePoint(u'×')
                              : MultiplicationSymbol(expression, linearMode()),
                          true);
      break;
    case Type::Pow:
    case Type::Div:
      layoutPowerOrDivision(parentRack, expression);
      break;
    case Type::Sub:
      layoutExpression(parentRack, child, k_subLeftChildPriority);
      child = child->nextTree();
      addOperatorSeparator(parentRack);
      PushCodePoint(parentRack, '-');
      addOperatorSeparator(parentRack);
      layoutExpression(parentRack, child, OperatorPriority(type));
      break;
    case Type::Opposite: {
      PushCodePoint(parentRack, '-');
      // Add extra parentheses for -1^2 -> -(1^2) but not for -x^2
      bool addExtraParenthesis =
          expression->child(0)->isPow() &&
          !expression->child(0)->child(0)->isUserSymbol();
      layoutExpression(
          parentRack, child,
          addExtraParenthesis ? k_forceParentheses : OperatorPriority(type));
      break;
    }
    case Type::Fact:
      layoutExpression(parentRack, child, OperatorPriority(type));
      PushCodePoint(parentRack, '!');
      break;
    case Type::PercentAddition:
      layoutExpression(parentRack, child, OperatorPriority(type));
      child = child->nextTree();
      if (child->isOpposite()) {
        PushCodePoint(parentRack, UCodePointSouthEastArrow);
        child = child->nextNode();
      } else {
        PushCodePoint(parentRack, UCodePointNorthEastArrow);
      }
      [[fallthrough]];
    case Type::PercentSimple:
      /* Use OperatorPriority(Type::PercentSimple) instead of
       * OperatorPriority(type) because PercentAddition's second
       * child has the same priority as a PercentSimple. */
      layoutExpression(parentRack, child,
                       OperatorPriority(Type::PercentSimple));
      PushCodePoint(parentRack, '%');
      break;
    case Type::EuclideanDivision:
      layoutInfixOperator(parentRack, expression,
                          CodePoint(UCodePointAssertion));
      break;
    case Type::EuclideanDivisionResult:
      PushCodePoint(parentRack, 'Q');
      PushCodePoint(parentRack, '=');
      layoutExpression(parentRack, child, k_commaPriority);
      child = child->nextTree();
      PushCodePoint(parentRack, ',');
      PushCodePoint(parentRack, 'R');
      PushCodePoint(parentRack, '=');
      layoutExpression(parentRack, child, k_commaPriority);
      break;
    case Type::Zero:
    case Type::MinusOne:
    case Type::One:
    case Type::Two:
    case Type::IntegerPosShort:
    case Type::IntegerNegShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
      layoutIntegerHandler(parentRack, Integer::Handler(expression));
      break;
    case Type::Half:
    case Type::RationalPosShort:
    case Type::RationalNegShort:
    case Type::RationalPosBig:
    case Type::RationalNegBig: {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
      layoutIntegerHandler(parentRack, Rational::Numerator(expression));
      PushCodePoint(parentRack, '/');
      layoutIntegerHandler(parentRack, Rational::Denominator(expression));
#else
      // Expression should have been beautified before layouting
      assert(false);
#endif
      break;
    }
    case Type::Diff:
      // TODO_PCJ: isValidCondensedForm and createValidExpandedForm
      // Case 1: f'(a)
      if (expression->child(0)->treeIsIdenticalTo(
              Derivation::k_functionDerivativeVariable)) {
        assert(expression->lastChild()->isUserFunction() &&
               expression->lastChild()->child(0)->isUserSymbol() &&
               expression->lastChild()->child(0)->treeIsIdenticalTo(
                   Derivation::k_functionDerivativeVariable));
        layoutText(parentRack, Symbol::GetName(expression->lastChild()));
        assert(expression->child(2)->isInteger() &&
               Integer::Handler(expression->child(2)).is<uint8_t>());
        int order = Integer::Handler(expression->child(2)).to<int>();
        if (order <= 2) {
          PushCodePoint(parentRack, order == 1
                                        ? Derivation::k_firstOrderSymbol
                                        : Derivation::k_secondOrderSymbol);
        } else {
          TreeRef rack;
          if (linearMode()) {
            PushCodePoint(parentRack, '^');
            rack = parentRack;
          } else {
            Tree* createdLayout = KSuperscriptL->cloneNode();
            rack = SharedTreeStack->pushRackLayout(0);
            NAry::AddChild(parentRack, createdLayout);
          }
          layoutExpression(rack, expression->child(2), k_forceParentheses);
        }
        layoutExpression(parentRack, expression->child(1), k_forceParentheses);
        break;
      }
      // Case 2: diff(f(x),x,a,n)
      if (linearMode()) {
        layoutBuiltin(parentRack, expression);
      } else {
        Tree* layout =
            (expression->child(2)->isOne() ? KDiffL : KNthDiffL)->cloneNode();
        layoutChildrenAsRacks(expression);
        NAry::AddChild(parentRack, layout);
      }
      break;
    case Type::Binomial:
    case Type::Permute:
      if (linearMode() || SharedPreferences->combinatoricSymbols() ==
                              Preferences::CombinatoricSymbols::Default) {
        layoutBuiltin(parentRack, expression);
      } else {
        Tree* layout = SharedTreeStack->pushBlock(
            type.isBinomial() ? Type::PtBinomialLayout : Type::PtPermuteLayout);
        layoutChildrenAsRacks(expression);
        NAry::AddChild(parentRack, layout);
      }
      break;
    case Type::LogBase: {
      if (linearMode()) {
        layoutBuiltin(parentRack, expression);
        break;
      }
      constexpr const char* log =
          Builtin::GetReservedFunction(Type::LogBase)->aliases()->mainAlias();
      bool nlLog = SharedPreferences->logarithmBasePosition() ==
                   Preferences::LogarithmBasePosition::TopLeft;
      if (!nlLog) {
        layoutText(parentRack, log);
      }
      // Base
      TreeRef layout =
          nlLog ? KPrefixSuperscriptL->cloneNode() : KSubscriptL->cloneNode();
      Tree* newParent = KRackL()->cloneTree();
      layoutExpression(newParent, expression->child(1), k_maxPriority);
      NAry::AddChild(parentRack, layout);
      if (nlLog) {
        layoutText(parentRack, log);
      }
      // Value
      layoutExpression(parentRack, child, k_forceParentheses);
      break;
    }
    case Type::MixedFraction:
      layoutExpression(parentRack, child, OperatorPriority(type));
      child = child->nextTree();
      if (linearMode()) {
        // TODO_PCJ make sure the serializer makes the distinction too
        PushCodePoint(parentRack, ' ');
      }
      layoutExpression(parentRack, child, OperatorPriority(type));
      break;
    case Type::PhysicalConstant:
      layoutText(parentRack, PhysicalConstant::GetProperties(expression)
                                 .m_aliasesList.mainAlias());
      break;
    case Type::UserSymbol:
    case Type::UserSequence:
    case Type::UserFunction: {
      layoutText(parentRack, Symbol::GetName(expression));
      if (type.isUserFunction()) {
        layoutExpression(parentRack, child, k_forceParentheses);
        child = child->nextTree();
      }
      if (type.isUserSequence()) {
        if (linearMode()) {
          layoutExpression(parentRack, child, k_forceParentheses);
        } else {
          Tree* layout = KSubscriptL->cloneNode();
          layoutChildrenAsRacks(expression);
          NAry::AddChild(parentRack, layout);
        }
      }
      break;
    }
    case Type::Matrix:
      layoutMatrix(parentRack, expression);
      break;
    case Type::Piecewise:
      if (linearMode()) {
        layoutBuiltin(parentRack, expression);
      } else {
        int rows = (expression->numberOfChildren() + 1) / 2;
        Tree* layout = SharedTreeStack->pushPiecewiseLayout(rows + 1, 2);
        layoutChildrenAsRacks(expression);
        // Placeholders
        if (expression->numberOfChildren() % 2 == 1) {
          KRackL()->cloneTree();
        }
        KRackL()->cloneTree();
        KRackL()->cloneTree();
        NAry::AddChild(parentRack, layout);
      }
      break;
    case Type::SequenceExplicit:
    case Type::SequenceSingleRecurrence:
    case Type::SequenceDoubleRecurrence:
      layoutSequence(parentRack, expression);
      break;
    case Type::EmptySequenceExpression:
      break;
    case Type::Unit:
      layoutUnit(parentRack, expression);
      break;
    case Type::Decimal:
    case Type::SingleFloat:
    case Type::DoubleFloat: {
      constexpr size_t bufferSize = 100;
      char buffer[bufferSize];
      serializeDecimalOrFloat(expression, buffer, bufferSize);
      Tree* rack = KRackL()->cloneTree();
      layoutText(rack, buffer);
      if (!linearMode()) {
        AddThousandsSeparators(rack);
      }
      NAry::AddOrMergeChild(parentRack, rack);
      break;
    }
    case Type::LogicalAnd:
    case Type::LogicalOr:
    case Type::LogicalXor:
    case Type::LogicalNot:
    case Type::LogicalNor:
    case Type::LogicalNand:
      if (!type.isLogicalNot()) {
        layoutExpression(parentRack, child, OperatorPriority(type));
        child = child->nextTree();
        PushCodePoint(parentRack, ' ');
      }
      layoutText(parentRack, Binary::OperatorName(type));
      PushCodePoint(parentRack, ' ');
      layoutExpression(parentRack, child, OperatorPriority(type));
      break;
    case Type::Equal:
    case Type::NotEqual:
    case Type::InferiorEqual:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::Superior:
      layoutExpression(parentRack, child, OperatorPriority(type));
      child = child->nextTree();
      addOperatorSeparator(parentRack);
      if (type == Type::NotEqual) {
        // Special case for ≠ combined
        PushCombinedCodePoint(parentRack, CodePoint('='),
                              UCodePointCombiningLongSolidusOverlay);
      } else {
        layoutText(parentRack, Comparison::OperatorString(
                                   Binary::ComparisonOperatorForType(type)));
      }

      addOperatorSeparator(parentRack);
      layoutExpression(parentRack, child, OperatorPriority(type));
      break;
    case Type::List:
    case Type::DepList:
    case Type::Point: {
      Tree* newParent = insertParenthesis(parentRack, true, !type.isPoint());
      layoutInfixOperator(newParent, expression, ',');
      insertParenthesis(newParent, false, !type.isPoint());
      break;
    }
    case Type::ListElement:
      layoutExpression(parentRack, child, OperatorPriority(type));
      child = child->nextTree();
      layoutExpression(parentRack, child, k_forceParentheses);
      break;
    case Type::ListSlice: {
      layoutExpression(parentRack, child, OperatorPriority(type));
      child = child->nextTree();
      Tree* newParent = insertParenthesis(parentRack, true);
      layoutExpression(newParent, child, OperatorPriority(type));
      child = child->nextTree();
      PushCodePoint(newParent, ',');
      layoutExpression(newParent, child, OperatorPriority(type));
      insertParenthesis(newParent, false);
      break;
    }
    case Type::Parentheses:
      layoutExpression(parentRack, child, k_forceParentheses);
      break;
    case Type::Store:
    case Type::UnitConversion:
      layoutInfixOperator(parentRack, expression, UCodePointRightwardsArrow);
      break;
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
    case Type::Placeholder: {
      PushCodePoint(parentRack, 'K');
      uint8_t offset = Placeholder::NodeToTag(expression);
      Placeholder::Filter filter = Placeholder::NodeToFilter(expression);
      char name = 'A' + offset;
      PushCodePoint(parentRack, name);
      if (filter != Placeholder::Filter::One) {
        PushCodePoint(parentRack, '_');
        PushCodePoint(parentRack,
                      filter == Placeholder::Filter::ZeroOrMore ? 's' : 'p');
      }
      break;
    }
    case Type::Var: {
      uint8_t offset = Variables::Id(expression);
      do {
        uint8_t localOffset = offset % 24;
        char name = 'a' + localOffset;
        // Skip e and i
        if (name >= 'e') {
          name++;
        }
        if (name >= 'i') {
          name++;
        }
        PushCodePoint(parentRack, name);
        offset = (offset - localOffset) / 24;
      } while (offset != 0);
      break;
    }
#endif
    case Type::Polynomial:
    default:
      if (Builtin::IsReservedFunction(expression)) {
        layoutBuiltin(parentRack, expression);
      } else if (Builtin::HasSpecialIdentifier(type)) {
        layoutText(parentRack, Builtin::SpecialIdentifierName(
                                   type, SharedPreferences->translateBuiltins())
                                   .mainAlias());
      } else {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
        layoutFunctionCall(
            parentRack, expression,
            TypeBlock::names[static_cast<uint8_t>(*expression->block())]);
#else
        OMG::unreachable();
#endif
        break;
      }
  }
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
constexpr int k_minDigitsForThousandsSeparator = 5;
constexpr int k_minValueForThousandsSeparator = 10000;

bool Layouter::AddThousandsSeparators(Tree* rack) {
  int nonDigitIndex = FirstNonDigitIndex(rack);
  bool isNegative = rack->child(0)->isCodePointLayout() &&
                    CodePointLayout::GetCodePoint(rack->child(0)) == '-';
  if (nonDigitIndex - isNegative < k_minDigitsForThousandsSeparator) {
    return false;
  }
  int index = isNegative + 1;  // skip "-" and first digit
  Tree* digit = rack->child(index);
  while (index < nonDigitIndex) {
    if ((nonDigitIndex - index) % 3 == 0) {
      digit->cloneTreeAtNode(KThousandsSeparatorL);
      digit = digit->nextTree();
    }
    digit = digit->nextTree();
    index++;
  }
  NAry::SetNumberOfChildren(
      rack, rack->numberOfChildren() + (nonDigitIndex - isNegative - 1) / 3);
  return true;
}

bool Layouter::requireSeparators(const Tree* expression) {
  if (expression->isRational()) {
    IntegerHandler num = Rational::Numerator(expression);
    num.setSign(NonStrictSign::Positive);
    if (IntegerHandler::Compare(num, k_minValueForThousandsSeparator) >= 0) {
      return true;
    }
    IntegerHandler den = Rational::Denominator(expression);
    if (IntegerHandler::Compare(den, k_minValueForThousandsSeparator) >= 0) {
      return true;
    }
    return false;
  }
  if (expression->isFloat() || expression->isDecimal()) {
    /* Since rules are complex with floatDisplayMode, layout the float and check
     * if it has separators. */
    Tree* clone = expression->cloneTree();
    Tree* rack = KRackL()->cloneTree();
    layoutExpression(rack, clone, k_tokenPriority);
    bool found = false;
    for (const Tree* child : rack->descendants()) {
      if (child->isSeparatorLayout()) {
        found = true;
        break;
      }
    }
    SharedTreeStack->dropBlocksFrom(clone);
    return found;
  }
  for (const Tree* child : expression->children()) {
    if (requireSeparators(child)) {
      return true;
    }
    if (expression->isMult() && Units::Unit::IsUnitOrPowerOfUnit(child)) {
      return true;
    }
  }
  return false;
}

void Layouter::StripSeparators(Tree* rack) {
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

void Layouter::StripUselessPlus(Tree* rack) {
  /* Ad-hoc method to turn "+-" and "+<separator>-" into "-" and "-<separator>"
   * respectively.
   * TODO: we should rather rework LayoutExpression(negative double) to make it
   * work like rationals with first the beautification into opposite block and
   * then the insertion of - instead of + when layouting the addition. */
  assert(rack->isRackLayout());
  Tree* child = rack->nextNode();
  int n = rack->numberOfChildren();
  int i = 0;
  while (i < n) {
    if (CodePointLayout::IsCodePoint(child, '+') && i + 1 < n) {
      Tree* next = child->nextTree();
      if (next->isOperatorSeparatorLayout() && i + 2 < n) {
        next = next->nextTree();
      }
      if (CodePointLayout::IsCodePoint(next, '-')) {
        /* Unary minus are not followed by a separator, reusing the potential
         * separator of the plus is sufficient. */
        child->moveTreeOverTree(next);
        n--;
      }
    }
    for (Tree* subRack : child->children()) {
      StripUselessPlus(subRack);
    }
    child = child->nextTree();
    i++;
  }
  NAry::SetNumberOfChildren(rack, n);
}

bool Layouter::implicitAddition(const Tree* addition) {
#if POINCARE_UNIT
  if (addition->numberOfChildren() < 2) {
    return false;
  }
  const Units::Representative* storedUnitRepresentative = nullptr;
  for (const Tree* child : addition->children()) {
    if (!(child->isMult() && child->numberOfChildren() == 2 &&
          (child->child(0)->isInteger() ||
           child->child(0)->isOfType(
               {Type::Decimal, Type::DoubleFloat, Type::SingleFloat})) &&
          child->child(1)->isUnitOrPhysicalConstant())) {
      return false;
    }
    ComplexProperties properties = GetComplexProperties(child->child(0));
    if (!properties.isReal() || !properties.realSign().isPositive()) {
      return false;
    }
    /*  Check if the layout of the addition contains an 'ᴇ'.
     * If it's the case, return false, since implicit
     * addition should not contain any 'ᴇ'.*/
    if (!child->child(0)->isInteger()) {
      constexpr size_t bufferSize = 100;
      char buffer[bufferSize];
      serializeDecimalOrFloat(child->child(0), buffer, bufferSize);
      if (UTF8Helper::HasCodePoint(buffer,
                                   UCodePointLatinLetterSmallCapitalE)) {
        return false;
      }
    }
    const Units::Representative* childRepresentative =
        Units::Unit::GetRepresentative(child->child(1));
    // Check if units can be implicitly added
    if (storedUnitRepresentative &&
        !Units::Unit::AllowImplicitAddition(childRepresentative,
                                            storedUnitRepresentative)) {
      return false;
    }
    storedUnitRepresentative = childRepresentative;
  }
  return true;
#else
  return false;
#endif
}

}  // namespace Poincare::Internal
