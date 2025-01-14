#include "layouter.h"

#include <omg/unreachable.h>
#include <omg/utf8_helper.h>
#include <poincare/comparison_operator.h>
#include <poincare/print_float.h>
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
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/placeholder.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "grid.h"
#include "k_tree.h"
#include "multiplication_symbol.h"

namespace Poincare::Internal {

static constexpr int k_forceParentheses = -2;

// A single token will never need parentheses
static constexpr int k_tokenPriority = -1;

// MaxPriority is to be used when there is no parent that could cause confusion
static constexpr int k_maxPriority = 20;

/* Priority just after Add for left child of a subtraction that may be an
 * unparenthesed addition */
static constexpr int k_subLeftChildPriority = 8;

static constexpr int OperatorPriority(TypeBlock type) {
  switch (type) {
    case Type::Fact:
    case Type::PercentSimple:
      return 0;
    case Type::Pow:
      return 1;
    case Type::Div:
      return 2;
    case Type::EuclideanDivision:
      return 3;
    case Type::Mult:
      return 4;
    case Type::PercentAddition:
    case Type::MixedFraction:
      return 5;
    case Type::Opposite:
    // Opposite could be higher but we prefer to display 2^(-1) instead of 2^-1
    case Type::Sub:
      return 6;
    case Type::Add:
      return 7;
      static_assert(k_subLeftChildPriority == 8);

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
    case Type::Set:
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
static constexpr int k_commaPriority = OperatorPriority(Type::Set);

Tree* Layouter::LayoutExpression(Tree* expression, bool linearMode,
                                 int numberOfSignificantDigits,
                                 Preferences::PrintFloatMode floatMode,
                                 OMG::Base base) {
  ExceptionTryAfterBlock(expression) {
    return UnsafeLayoutExpression(expression, linearMode,
                                  numberOfSignificantDigits, floatMode, base);
  }
  ExceptionCatch(exc) {
    switch (exc) {
      case ExceptionType::TreeStackOverflow:
      case ExceptionType::IntegerOverflow:
        return UnsafeLayoutExpression(KUndef->cloneTree(), linearMode,
                                      numberOfSignificantDigits, floatMode,
                                      base);
      default:
        TreeStackCheckpoint::Raise(exc);
    }
  }
  OMG::unreachable();
}

Tree* Layouter::UnsafeLayoutExpression(Tree* expression, bool linearMode,
                                       int numberOfSignificantDigits,
                                       Preferences::PrintFloatMode floatMode,
                                       OMG::Base base) {
  assert(expression->isExpression() || expression->isPlaceholder());
  /* expression lives before layoutParent in the TreeStack and will be
   * destroyed in the process. A TreeRef is necessary to keep track of
   * layoutParent's root. */
  TreeRef layoutParent = SharedTreeStack->pushRackLayout(0);
  assert(expression->nextTree() == layoutParent);
  Layouter layouter(linearMode, false, numberOfSignificantDigits, floatMode,
                    base);
  layouter.m_addSeparators =
      !linearMode && layouter.requireSeparators(expression);
  layouter.layoutExpression(layoutParent, expression, k_maxPriority);
  StripUselessPlus(layoutParent);
  assert(expression == layoutParent);
  return layoutParent;
}

static void PushCodePoint(Tree* layout, CodePoint codePoint) {
  NAry::AddChild(layout, CodePointLayout::Push(codePoint));
}

static void PushCombinedCodePoint(Tree* layout, CodePoint codePoint,
                                  CodePoint combiningCodePoint) {
  NAry::AddChild(layout,
                 CodePointLayout::PushCombined(codePoint, combiningCodePoint));
}

static void InsertCodePointAt(Tree* layout, CodePoint codePoint, int index) {
  NAry::AddChildAtIndex(layout, CodePointLayout::Push(codePoint), index);
}

void Layouter::addOperatorSeparator(Tree* layoutParent) {
  if (!m_addSeparators) {
    return;
  }
  assert(!m_linearMode);
  NAry::AddChild(layoutParent, KOperatorSeparatorL->cloneTree());
}

void Layouter::addUnitSeparator(Tree* layoutParent) {
  if (!m_addSeparators) {
    return;
  }
  assert(!m_linearMode);
  NAry::AddChild(layoutParent, KUnitSeparatorL->cloneTree());
}

void Layouter::layoutText(TreeRef& layoutParent, const char* text) {
  UTF8Decoder decoder(text);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull) {
    PushCodePoint(layoutParent, codePoint);
    codePoint = decoder.nextCodePoint();
  }
}

void Layouter::layoutBuiltin(TreeRef& layoutParent, Tree* expression) {
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
    TreeRef layout =
        SharedTreeStack->pushBlock(builtinWithLayout->layoutType());
    layoutChildrenAsRacks(expression);
    NAry::AddChild(layoutParent, layout);
  }
}

void Layouter::layoutFunctionCall(TreeRef& layoutParent, Tree* expression,
                                  const char* name) {
  layoutText(layoutParent, name);
  TreeRef parenthesis = SharedTreeStack->pushParenthesisLayout(false, false);
  TreeRef newParent = SharedTreeStack->pushRackLayout(0);
  NAry::AddChild(layoutParent, parenthesis);
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    if (((j == 1 && expression->isListStatWithCoefficients()) ||
         (j == 3 && expression->isDiff())) &&
        expression->nextNode()->isOne()) {
      // TODO: factorize with PromoteBuiltin?
      // TODO: factorize with 2D layouting?
      /* Remove default parameters:
       * - mean(L, 1) -> mean(L)
       * - diff(f, x, y, 1) -> diff(f, x, y) */
      expression->nextNode()->removeTree();
      continue;
    }
    if (j != 0) {
      PushCodePoint(newParent, ',');
    }
    layoutExpression(newParent, expression->nextNode(), k_commaPriority);
  }
}

void Layouter::layoutChildrenAsRacks(Tree* expression) {
  for (int j = 0; j < expression->numberOfChildren(); j++) {
    TreeRef newParent = SharedTreeStack->pushRackLayout(0);
    layoutExpression(newParent, expression->nextNode(), k_maxPriority);
  }
}

void Layouter::layoutIntegerHandler(TreeRef& layoutParent,
                                    IntegerHandler handler, int decimalOffset) {
  if (handler.strictSign() == StrictSign::Negative) {
    PushCodePoint(layoutParent, '-');
  }
  handler.setSign(NonStrictSign::Positive);
  if (m_base != OMG::Base::Decimal) {
    assert(decimalOffset == 0);
    PushCodePoint(layoutParent, '0');
    PushCodePoint(layoutParent, m_base == OMG::Base::Binary ? 'b' : 'x');
  }
  Tree* rack = KRackL()->cloneTree();
  /* We can't manipulate an IntegerHandler in a workingBuffer since we're
   * pushing layouts on the TreeStack at each steps. Value is therefore
   * temporarily stored and updated on the TreeStack. */
  TreeRef value = handler.pushOnTreeStack();
  do {
    DivisionResult result = IntegerHandler::Division(
        Integer::Handler(value), IntegerHandler(static_cast<uint8_t>(m_base)));
    uint8_t digit = Integer::Handler(result.remainder);
    assert(result.remainder > result.quotient);
    result.remainder->removeTree();
    MoveTreeOverTree(value, result.quotient);
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
  if (m_base == OMG::Base::Decimal) {
    AddThousandsSeparators(rack);
  }
  NAry::AddOrMergeChild(layoutParent, rack);
}

void Layouter::layoutInfixOperator(TreeRef& layoutParent, Tree* expression,
                                   CodePoint op, bool multiplication) {
  Type type = expression->type();
  int operatorPriority = OperatorPriority(type);
  int childNumber = expression->numberOfChildren();
  bool previousWasUnit = false;
  for (int childIndex = 0; childIndex < childNumber; childIndex++) {
    Tree* child = expression->nextNode();
    bool isUnit = Units::Unit::IsUnitOrPowerOfUnit(child);
    if (childIndex > 0) {
      if (!m_linearMode && multiplication && isUnit) {
        if (!previousWasUnit) {
          if (Units::Unit::ForceMarginLeftOfUnit(child)) {
            // Add small separator between 2 and m in "2 m"
            addUnitSeparator(layoutParent);
          }
        } else {
          PushCodePoint(layoutParent, UCodePointMiddleDot);
        }
        layoutExpression(layoutParent, child, operatorPriority);
        previousWasUnit = isUnit;
        continue;
      }
      addOperatorSeparator(layoutParent);
      if (op != UCodePointNull) {
        if (op == '+' && child->isOpposite()) {
          // Consume opposite block now and insert - instead of +
          PushCodePoint(layoutParent, '-');
          child->removeNode();
          if (OperatorPriority(Type::Opposite) <
              OperatorPriority(child->type())) {
            // Add(A, Oppose(Add(A, B))) -> A - ( B + C )
            operatorPriority = k_forceParentheses;
          }
        } else {
          PushCodePoint(layoutParent, op);
        }
        addOperatorSeparator(layoutParent);
      }
    }
    layoutExpression(layoutParent, child, operatorPriority);
    previousWasUnit = isUnit;
  }
}

void Layouter::layoutMatrix(TreeRef& layoutParent, Tree* expression) {
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

void Layouter::layoutUnit(TreeRef& layoutParent, Tree* expression) {
  // TODO_PCJ: ask the context whether to add an underscore
  if (m_linearMode) {
    PushCodePoint(layoutParent, '_');
  }
  layoutText(layoutParent, Units::Unit::GetPrefix(expression)->symbol());
  layoutText(
      layoutParent,
      Units::Unit::GetRepresentative(expression)->rootSymbols().mainAlias());
}

void Layouter::layoutPowerOrDivision(TreeRef& layoutParent, Tree* expression) {
  Type type = expression->type();
  /* Once first child has been converted, this will point to second child. */
  expression = expression->child(0);
  TreeRef createdLayout;
  // No parentheses in Fraction roots and Power index.
  if (m_linearMode) {
    // force parentheses when serializing e^(x)
    int secondChildPriority = type == Type::Pow && expression->isEulerE()
                                  ? k_forceParentheses
                                  : OperatorPriority(type);
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    PushCodePoint(layoutParent, type == Type::Div ? '/' : '^');
    layoutExpression(layoutParent, expression, secondChildPriority);
    return;
  }
  if (type == Type::Div) {
    createdLayout = SharedTreeStack->pushFractionLayout();
    TreeRef rack = SharedTreeStack->pushRackLayout(0);
    layoutExpression(rack, expression, k_maxPriority);
  } else {
    assert(type == Type::Pow || type == Type::PowMatrix);
    layoutExpression(layoutParent, expression, OperatorPriority(type));
    createdLayout = KSuperscriptL->cloneNode();
  }
  TreeRef rack = SharedTreeStack->pushRackLayout(0);
  layoutExpression(rack, expression, k_maxPriority);
  NAry::AddChild(layoutParent, createdLayout);
}

void Layouter::serializeDecimalOrFloat(const Tree* expression, char* buffer,
                                       size_t bufferSize) {
  assert(expression->isOfType(
      {Type::Decimal, Type::DoubleFloat, Type::SingleFloat}));
  int numberOfSignificantDigits =
      m_numberOfSignificantDigits != -1 ? m_numberOfSignificantDigits
      : expression->isSingleFloat()
          ? Poincare::PrintFloat::SignificantDecimalDigits<float>()
          : Poincare::PrintFloat::SignificantDecimalDigits<double>();
  if (expression->isDecimal()) {
    Decimal::Serialize(expression, buffer, bufferSize, m_floatMode,
                       numberOfSignificantDigits);
  } else {
    Poincare::PrintFloat::ConvertFloatToText(
        FloatHelper::To(expression), buffer, bufferSize,
        Poincare::PrintFloat::k_maxFloatGlyphLength, numberOfSignificantDigits,
        m_floatMode);
  }
}

// Remove expression while converting it to a layout in layoutParent
void Layouter::layoutExpression(TreeRef& layoutParent, Tree* expression,
                                int parentPriority) {
  assert(layoutParent->isRackLayout());
  TypeBlock type = expression->type();

  // Add Parentheses if necessary
  if (parentPriority <= OperatorPriority(type) &&
      (!(type.isPoint() || type.isDepList() || type.isList()) ||
       parentPriority == k_forceParentheses)) {
    TreeRef parenthesis = KParenthesesL(KRackL())->cloneTree();
    NAry::AddChild(layoutParent, parenthesis);
    TreeRef rack = parenthesis->child(0);
    return layoutExpression(rack, expression, k_maxPriority);
  }

  switch (type) {
    // TODO_PCJ: Restore Addition and Multiplication symbol in linear mode
    case Type::Add: {
      CodePoint op = implicitAddition(expression) && !m_linearMode
                         ? UCodePointNull
                         : CodePoint('+');
      layoutInfixOperator(layoutParent, expression, op);
      break;
    }
    case Type::Mult:
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
                       k_subLeftChildPriority);
      addOperatorSeparator(layoutParent);
      PushCodePoint(layoutParent, '-');
      addOperatorSeparator(layoutParent);
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
          addExtraParenthesis ? k_forceParentheses : OperatorPriority(type));
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
      [[fallthrough]];
    case Type::PercentSimple:
      /* Use OperatorPriority(Type::PercentSimple) instead of
       * OperatorPriority(type) because PercentAddition's second
       * child has the same priority as a PercentSimple. */
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(Type::PercentSimple));
      PushCodePoint(layoutParent, '%');
      break;
    case Type::EuclideanDivision:
      layoutInfixOperator(layoutParent, expression,
                          CodePoint(UCodePointAssertion));
      break;
    case Type::EuclideanDivisionResult:
      PushCodePoint(layoutParent, 'Q');
      PushCodePoint(layoutParent, '=');
      layoutExpression(layoutParent, expression->nextNode(), k_commaPriority);
      PushCodePoint(layoutParent, ',');
      PushCodePoint(layoutParent, 'R');
      PushCodePoint(layoutParent, '=');
      layoutExpression(layoutParent, expression->nextNode(), k_commaPriority);
      break;
    case Type::Zero:
    case Type::MinusOne:
    case Type::One:
    case Type::Two:
    case Type::IntegerPosShort:
    case Type::IntegerNegShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
      layoutIntegerHandler(layoutParent, Integer::Handler(expression));
      break;
    case Type::Half:
    case Type::RationalPosShort:
    case Type::RationalNegShort:
    case Type::RationalPosBig:
    case Type::RationalNegBig: {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
      layoutIntegerHandler(layoutParent, Rational::Numerator(expression));
      PushCodePoint(layoutParent, '/');
      layoutIntegerHandler(layoutParent, Rational::Denominator(expression));
#else
      // Expression should have been beautified before layouting
      assert(false);
#endif
      break;
    }
    case Type::Diff:
      // TODO_PCJ: isValidCondensedForm and createValidExpandedForm
      // Case 1: f'(a)
      if (expression->child(0)->treeIsIdenticalTo(KUnknownSymbol)) {
        assert(expression->lastChild()->isUserFunction() &&
               expression->lastChild()->child(0)->isUserSymbol() &&
               expression->lastChild()->child(0)->treeIsIdenticalTo(
                   KUnknownSymbol));
        layoutText(layoutParent, Symbol::GetName(expression->lastChild()));
        int order = Integer::Handler(expression->child(2)).to<int>();
        assert(order > 0);
        if (order <= 2) {
          PushCodePoint(layoutParent, order == 1
                                          ? Derivation::k_firstOrderSymbol
                                          : Derivation::k_secondOrderSymbol);
          expression->child(2)->removeTree();
        } else {
          TreeRef rack;
          if (m_linearMode) {
            PushCodePoint(layoutParent, '^');
            rack = layoutParent;
          } else {
            Tree* createdLayout = KSuperscriptL->cloneNode();
            rack = SharedTreeStack->pushRackLayout(0);
            NAry::AddChild(layoutParent, createdLayout);
          }
          layoutExpression(rack, expression->child(2), k_forceParentheses);
        }
        expression->child(2)->removeTree();
        expression->child(0)->removeTree();
        layoutExpression(layoutParent, expression->nextNode(),
                         k_forceParentheses);
        break;
      }
      // Case 2: diff(f(x),x,a,n)
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        TreeRef layout =
            (expression->child(2)->isOne() ? KDiffL : KNthDiffL)->cloneNode();
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
        TreeRef layout = SharedTreeStack->pushBlock(
            type.isBinomial() ? Type::PtBinomialLayout : Type::PtPermuteLayout);
        layoutChildrenAsRacks(expression);
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case Type::LogBase: {
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
        break;
      }
      constexpr const char* log =
          Builtin::GetReservedFunction(Type::LogBase)->aliases()->mainAlias();
      bool nlLog = Preferences::SharedPreferences()->logarithmBasePosition() ==
                   Preferences::LogarithmBasePosition::TopLeft;
      if (!nlLog) {
        layoutText(layoutParent, log);
      }
      // Base
      TreeRef layout =
          nlLog ? KPrefixSuperscriptL->cloneNode() : KSubscriptL->cloneNode();
      TreeRef newParent = KRackL()->cloneTree();
      layoutExpression(newParent, expression->child(1), k_maxPriority);
      NAry::AddChild(layoutParent, layout);
      if (nlLog) {
        layoutText(layoutParent, log);
      }
      // Value
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParentheses);
      break;
    }
    case Type::MixedFraction:
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      if (m_linearMode) {
        // TODO_PCJ make sure the serializer makes the distinction too
        PushCodePoint(layoutParent, ' ');
      }
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case Type::PhysicalConstant:
      layoutText(layoutParent, PhysicalConstant::GetProperties(expression)
                                   .m_aliasesList.mainAlias());
      break;
    case Type::UserSymbol:
    case Type::UserSequence:
    case Type::UserFunction: {
      layoutText(layoutParent, Symbol::GetName(expression));
      if (type.isUserFunction()) {
        // minimum priority to force parentheses
        layoutExpression(layoutParent, expression->nextNode(),
                         k_forceParentheses);
      }
      if (type.isUserSequence()) {
        if (m_linearMode) {
          layoutExpression(layoutParent, expression->nextNode(),
                           k_forceParentheses);
        } else {
          TreeRef layout = KSubscriptL->cloneNode();
          layoutChildrenAsRacks(expression);
          NAry::AddChild(layoutParent, layout);
        }
      }
      break;
    }
    case Type::Matrix:
      layoutMatrix(layoutParent, expression);
      break;
    case Type::Piecewise:
      if (m_linearMode) {
        layoutBuiltin(layoutParent, expression);
      } else {
        int rows = (expression->numberOfChildren() + 1) / 2;
        TreeRef layout = SharedTreeStack->pushPiecewiseLayout(rows + 1, 2);
        layoutChildrenAsRacks(expression);
        // Placeholders
        if (expression->numberOfChildren() % 2 == 1) {
          KRackL()->cloneTree();
        }
        KRackL()->cloneTree();
        KRackL()->cloneTree();
        NAry::AddChild(layoutParent, layout);
      }
      break;
    case Type::Unit:
      layoutUnit(layoutParent, expression);
      break;
    case Type::Decimal:
    case Type::SingleFloat:
    case Type::DoubleFloat: {
      constexpr size_t bufferSize = 100;
      char buffer[bufferSize];
      serializeDecimalOrFloat(expression, buffer, bufferSize);
      if (type.isDecimal()) {
        expression->child(1)->removeTree();
        expression->child(0)->removeTree();
      }
      TreeRef rack = KRackL()->cloneTree();
      layoutText(rack, buffer);
      AddThousandsSeparators(rack);
      NAry::AddOrMergeChild(layoutParent, rack);
      break;
    }
    case Type::LogicalAnd:
    case Type::LogicalOr:
    case Type::LogicalXor:
    case Type::LogicalNot:
    case Type::LogicalNor:
    case Type::LogicalNand:
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
      addOperatorSeparator(layoutParent);
      if (type == Type::NotEqual) {
        // Special case for ≠ combined
        PushCombinedCodePoint(layoutParent, CodePoint('='),
                              UCodePointCombiningLongSolidusOverlay);
      } else {
        layoutText(layoutParent, ComparisonJunior::OperatorString(
                                     Binary::ComparisonOperatorForType(type)));
      }

      addOperatorSeparator(layoutParent);
      layoutExpression(layoutParent, expression->nextNode(),
                       OperatorPriority(type));
      break;
    case Type::List:
    case Type::Set:
    case Type::DepList:
    case Type::Point:
      if (m_linearMode) {
        PushCodePoint(layoutParent, type.isPoint() ? '(' : '{');
        layoutInfixOperator(layoutParent, expression, ',');
        PushCodePoint(layoutParent, type.isPoint() ? ')' : '}');
        break;
      } else {
        TreeRef parenthesis =
            (type.isList() ? KCurlyBracesL : KParenthesesL)->cloneNode();
        TreeRef rack = KRackL()->cloneTree();
        NAry::AddChild(layoutParent, parenthesis);
        layoutInfixOperator(rack, expression, ',');
        break;
      }
    case Type::Parentheses:
      layoutExpression(layoutParent, expression->nextNode(),
                       k_forceParentheses);
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
        PushCodePoint(layoutParent, name);
        offset = (offset - localOffset) / 24;
      } while (offset != 0);
      break;
    }
#endif
    case Type::Polynomial:
    default:
      if (Builtin::IsReservedFunction(expression)) {
        layoutBuiltin(layoutParent, expression);
      } else if (Builtin::HasSpecialIdentifier(type)) {
        layoutText(layoutParent,
                   Builtin::SpecialIdentifierName(type).mainAlias());
      } else {
#if POINCARE_TREE_LOG  // Improves Tree::logSerialize
        layoutFunctionCall(
            layoutParent, expression,
            TypeBlock::names[static_cast<uint8_t>(*expression->block())]);
#else
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
    TreeRef rack = KRackL()->cloneTree();
    layoutExpression(rack, clone, k_tokenPriority);
    for (const Tree* child : rack->descendants()) {
      if (child->isSeparatorLayout()) {
        rack->removeTree();
        return true;
      }
    }
    rack->removeTree();
    return false;
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
    ComplexSign sign = GetComplexSign(child->child(0));
    if (!sign.isReal() || !sign.realSign().isPositive()) {
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
