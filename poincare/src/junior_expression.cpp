#include <poincare/junior_expression.h>
#include <poincare/junior_layout.h>
#include <poincare/matrix.h>
#include <poincare/point_2D_layout.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/conversion.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/unit.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/n_ary.h>

namespace Poincare {

/* JuniorExpressionNode */

int JuniorExpressionNode::simplificationOrderSameType(
    const ExpressionNode* e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(otype() == e->otype());
  return PoincareJ::Comparison::Compare(
      tree(), static_cast<const JuniorExpressionNode*>(e)->tree());
}

Evaluation<float> JuniorExpressionNode::approximate(
    SinglePrecision p, const ApproximationContext& approximationContext) const {
  // TODO_PCJ: Handle Matrix, list, boolean and point evaluations. Use p.
  return Complex<float>::Builder(
      PoincareJ::Approximation::RootTreeToComplex<float>(
          tree(),
          static_cast<PoincareJ::AngleUnit>(approximationContext.angleUnit()),
          static_cast<PoincareJ::ComplexFormat>(
              approximationContext.complexFormat())));
}

Evaluation<double> JuniorExpressionNode::approximate(
    DoublePrecision p, const ApproximationContext& approximationContext) const {
  // TODO_PCJ: Handle Matrix, list, boolean and point evaluations. Use p.
  return Complex<double>::Builder(
      PoincareJ::Approximation::RootTreeToComplex<double>(
          tree(),
          static_cast<PoincareJ::AngleUnit>(approximationContext.angleUnit()),
          static_cast<PoincareJ::ComplexFormat>(
              approximationContext.complexFormat())));
}

Layout JuniorExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return JuniorLayout::Builder(PoincareJ::Layoutter::LayoutExpression(
      tree()->clone(), false, numberOfSignificantDigits, floatDisplayMode));
}

size_t JuniorExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  OExpression e = PoincareJ::ToPoincareExpression(tree());
  return e.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

/* JuniorExpression */

JuniorExpression JuniorExpression::Parse(const PoincareJ::Tree* layout,
                                         Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  // TODO_PCJ: Use context, addMissingParenthesis and parseForAssignment.
  return Builder(PoincareJ::Parser::Parse(layout));
}

JuniorExpression JuniorExpression::Parse(char const* string, Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  if (string[0] == 0) {
    return JuniorExpression();
  }
  PoincareJ::Tree* layout = PoincareJ::RackFromText(string);
  if (!layout) {
    return JuniorExpression();
  }
  JuniorExpression result =
      Parse(layout, context, addMissingParenthesis, parseForAssignment);
  layout->removeTree();
  return result;
}

JuniorExpression JuniorExpression::Builder(const PoincareJ::Tree* tree) {
  if (!tree) {
    return JuniorExpression();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorExpressionNode) + size);
  JuniorExpressionNode* node =
      new (bufferNode) JuniorExpressionNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorExpression&>(h);
}

JuniorExpression JuniorExpression::Builder(PoincareJ::Tree* tree) {
  JuniorExpression result = Builder(const_cast<const PoincareJ::Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

JuniorExpression JuniorExpression::Juniorize(OExpression e) {
  if (e.isUninitialized() ||
      e.otype() == ExpressionNode::Type::JuniorExpression) {
    // e is already a junior expression
    return static_cast<JuniorExpression&>(e);
  }
  return Builder(PoincareJ::FromPoincareExpression(e));
}

JuniorExpression JuniorExpression::childAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

ExpressionNode::Type JuniorExpression::type() const {
  /* TODO_PCJ: These are the types checked for in apps. Update apps to use the
   *           new blockType instead. */
  if (tree()->isRational()) {
    return tree()->isInteger() ? ExpressionNode::Type::BasedInteger
                               : ExpressionNode::Type::Rational;
  }
  switch (tree()->type()) {
    case PoincareJ::BlockType::Addition:
      return ExpressionNode::Type::Addition;
    case PoincareJ::BlockType::True:
    case PoincareJ::BlockType::False:
      return ExpressionNode::Type::Boolean;
    case PoincareJ::BlockType::ComplexArgument:
      return ExpressionNode::Type::ComplexArgument;
    case PoincareJ::BlockType::Conjugate:
      return ExpressionNode::Type::Conjugate;
    case PoincareJ::BlockType::PhysicalConstant:
      return ExpressionNode::Type::ConstantPhysics;
    case PoincareJ::BlockType::Dependency:
      return ExpressionNode::Type::Dependency;
    case PoincareJ::BlockType::Derivative:
      return ExpressionNode::Type::Derivative;
    case PoincareJ::BlockType::Division:
      return ExpressionNode::Type::Division;
    case PoincareJ::BlockType::Factor:
      return ExpressionNode::Type::Factor;
    case PoincareJ::BlockType::FracPart:
      return ExpressionNode::Type::FracPart;
    case PoincareJ::BlockType::ImaginaryPart:
      return ExpressionNode::Type::ImaginaryPart;
    case PoincareJ::BlockType::Infinity:
      return ExpressionNode::Type::Infinity;
    case PoincareJ::BlockType::Integral:
      return ExpressionNode::Type::Integral;
    case PoincareJ::BlockType::List:
      return ExpressionNode::Type::List;
    case PoincareJ::BlockType::ListSequence:
      return ExpressionNode::Type::ListSequence;
    case PoincareJ::BlockType::Logarithm:
      return ExpressionNode::Type::Logarithm;
    case PoincareJ::BlockType::Matrix:
      return ExpressionNode::Type::Matrix;
    case PoincareJ::BlockType::Multiplication:
      return ExpressionNode::Type::Multiplication;
    case PoincareJ::BlockType::Nonreal:
      return ExpressionNode::Type::Nonreal;
    case PoincareJ::BlockType::Opposite:
      return ExpressionNode::Type::Opposite;
    case PoincareJ::BlockType::Piecewise:
      return ExpressionNode::Type::PiecewiseOperator;
    case PoincareJ::BlockType::Point:
      return ExpressionNode::Type::Point;
    case PoincareJ::BlockType::Power:
      return ExpressionNode::Type::Power;
    case PoincareJ::BlockType::Product:
      return ExpressionNode::Type::Product;
    case PoincareJ::BlockType::RandInt:
      return ExpressionNode::Type::Randint;
    case PoincareJ::BlockType::RandIntNoRep:
      return ExpressionNode::Type::RandintNoRepeat;
    case PoincareJ::BlockType::Random:
      return ExpressionNode::Type::Random;
    case PoincareJ::BlockType::RealPart:
      return ExpressionNode::Type::RealPart;
    case PoincareJ::BlockType::Round:
      return ExpressionNode::Type::Round;
    case PoincareJ::BlockType::Store:
      return ExpressionNode::Type::Store;
    case PoincareJ::BlockType::Sum:
      return ExpressionNode::Type::Sum;
    case PoincareJ::BlockType::Undefined:
      return ExpressionNode::Type::Undefined;
    case PoincareJ::BlockType::UnitConversion:
      return ExpressionNode::Type::UnitConvert;
#if 0
      // No perfect PoincareJ equivalents
      return ExpressionNode::Type::Comparison;
      return ExpressionNode::Type::ConstantMaths;
      return ExpressionNode::Type::DistributionDispatcher;
      return ExpressionNode::Type::Function;
      return ExpressionNode::Type::Parenthesis;
      return ExpressionNode::Type::Sequence;
      return ExpressionNode::Type::Symbol;
#endif
    default:
      assert(false);
      return ExpressionNode::Type::JuniorExpression;
  }
}

void JuniorExpression::cloneAndSimplifyAndApproximate(
    JuniorExpression* simplifiedExpression,
    JuniorExpression* approximateExpression,
    const ReductionContext& reductionContext,
    bool approximateKeepingSymbols) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximateExpression || approximateExpression->isUninitialized());

  // Step 1: we reduce the expression
  assert(reductionContext.target() == ReductionTarget::User);
  PoincareJ::ProjectionContext context = {
      .m_complexFormat =
          PoincareJ::ComplexFormat(reductionContext.complexFormat()),
      .m_angleUnit = PoincareJ::AngleUnit(reductionContext.angleUnit()),
      .m_strategy = approximateKeepingSymbols
                        ? PoincareJ::Strategy::ApproximateToFloat
                        : PoincareJ::Strategy::Default,
      .m_unitFormat = PoincareJ::UnitFormat(reductionContext.unitFormat())};
  PoincareJ::Tree* e = tree()->clone();
  PoincareJ::Simplification::Simplify(e, context);
  if (approximateExpression) {
    *approximateExpression = JuniorExpression::Builder(
        PoincareJ::Approximation::RootTreeToTree<double>(
            e, context.m_angleUnit, context.m_complexFormat));
  }
  *simplifiedExpression = JuniorExpression::Builder(e);
  return;
}

static bool IsIgnoredSymbol(const JuniorExpression* e,
                            JuniorExpression::IgnoredSymbols* ignoredSymbols) {
  if (e->type() != ExpressionNode::Type::Symbol) {
    return false;
  }
  while (ignoredSymbols) {
    assert(ignoredSymbols->head);
    if (ignoredSymbols->head->isIdenticalTo(*e)) {
      return true;
    }
    ignoredSymbols = reinterpret_cast<JuniorExpression::IgnoredSymbols*>(
        ignoredSymbols->tail);
  }
  return false;
}

bool JuniorExpression::recursivelyMatches(
    ExpressionTrinaryTest test, Context* context,
    SymbolicComputation replaceSymbols, void* auxiliary,
    IgnoredSymbols* ignoredSymbols) const {
  if (!context) {
    replaceSymbols = SymbolicComputation::DoNotReplaceAnySymbol;
  }
  if (IsIgnoredSymbol(this, ignoredSymbols)) {
    return false;
  }
  TrinaryBoolean testResult = test(*this, context, auxiliary);
  if (testResult == TrinaryBoolean::True) {
    return true;
  } else if (testResult == TrinaryBoolean::False) {
    return false;
  }
  assert(testResult == TrinaryBoolean::Unknown && !isUninitialized());

  // Handle dependencies, store, symbols and functions
  ExpressionNode::Type t = type();
  if (t == ExpressionNode::Type::Dependency) {
#if 0  // TODO_PCJ
    JuniorExpression e = *this;
    return static_cast<Dependency&>(e).dependencyRecursivelyMatches(
        test, context, replaceSymbols, auxiliary, ignoredSymbols);
#else
    assert(false);
    return false;
#endif
  }
  if (t == ExpressionNode::Type::Store) {
#if 0  // TODO_PCJ
    JuniorExpression e = *this;
    return static_cast<Store&>(e).storeRecursivelyMatches(
        test, context, replaceSymbols, auxiliary, ignoredSymbols);
#else
    assert(false);
    return false;
#endif
  }
  if (t == ExpressionNode::Type::Symbol ||
      t == ExpressionNode::Type::Function) {
    assert(replaceSymbols ==
               SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
           replaceSymbols ==
               SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
           // We need only those cases for now
           || replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol);
    if (replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol ||
        (replaceSymbols ==
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions &&
         t == ExpressionNode::Type::Symbol)) {
      return false;
    }
    assert(replaceSymbols ==
               SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
           t == ExpressionNode::Type::Function);
#if 0  // TODO_PCJ
    return SymbolAbstract::matches(convert<const SymbolAbstract>(), test,
                                   context, auxiliary, ignoredSymbols);
#else
    assert(false);
    return false;
#endif
  }

  const int childrenCount = this->numberOfChildren();
  bool isParametered = tree()->isParametric();
  // Run loop backwards to find lists and matrices quicker in NAry expressions
  for (int i = childrenCount - 1; i >= 0; i--) {
    if (isParametered && i == PoincareJ::Parametric::k_variableIndex) {
      continue;
    }
    // TODO: There's no need to clone the juniorExpression here.
    JuniorExpression childToAnalyze = childAtIndex(i);
    bool matches;
    if (isParametered && i == PoincareJ::Parametric::FunctionIndex(tree())) {
      JuniorExpression symbolExpr =
          childAtIndex(PoincareJ::Parametric::k_variableIndex);
      IgnoredSymbols updatedIgnoredSymbols = {.head = &symbolExpr,
                                              .tail = ignoredSymbols};
      matches = childToAnalyze.recursivelyMatches(
          test, context, replaceSymbols, auxiliary, &updatedIgnoredSymbols);
    } else {
      matches = childToAnalyze.recursivelyMatches(test, context, replaceSymbols,
                                                  auxiliary, ignoredSymbols);
    }
    if (matches) {
      return true;
    }
  }
  return false;
}

bool JuniorExpression::recursivelyMatches(
    ExpressionTest test, Context* context,
    SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const JuniorExpression e, Context* context,
                                     void* auxiliary) {
    ExpressionTest* trueTest = static_cast<ExpressionTest*>(auxiliary);
    return (*trueTest)(e, context) ? TrinaryBoolean::True
                                   : TrinaryBoolean::Unknown;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool JuniorExpression::recursivelyMatches(
    SimpleExpressionTest test, Context* context,
    SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const JuniorExpression e, Context* context,
                                     void* auxiliary) {
    SimpleExpressionTest* trueTest =
        static_cast<SimpleExpressionTest*>(auxiliary);
    return (*trueTest)(e) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool JuniorExpression::recursivelyMatches(ExpressionTestAuxiliary test,
                                          Context* context,
                                          SymbolicComputation replaceSymbols,
                                          void* auxiliary) const {
  struct Pack {
    ExpressionTestAuxiliary* test;
    void* auxiliary;
  };
  ExpressionTrinaryTest ternary = [](const JuniorExpression e, Context* context,
                                     void* pack) {
    ExpressionTestAuxiliary* trueTest =
        static_cast<ExpressionTestAuxiliary*>(static_cast<Pack*>(pack)->test);
    return (*trueTest)(e, context, static_cast<Pack*>(pack)->auxiliary)
               ? TrinaryBoolean::True
               : TrinaryBoolean::Unknown;
  };
  Pack pack{&test, auxiliary};
  return recursivelyMatches(ternary, context, replaceSymbols, &pack);
}

/* Matrix */

Matrix Matrix::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::Matrix>(0, 0));
  return static_cast<Matrix&>(expr);
}

void Matrix::setDimensions(int rows, int columns) {
  assert(rows * columns == tree()->numberOfChildren());
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::Matrix::SetNumberOfColumns(clone, columns);
  PoincareJ::Matrix::SetNumberOfRows(clone, rows);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
}

bool Matrix::isVector() const {
  const PoincareJ::Tree* t = tree();
  return PoincareJ::Matrix::NumberOfRows(t) == 1 ||
         PoincareJ::Matrix::NumberOfColumns(t) == 1;
}

int Matrix::numberOfRows() const {
  return PoincareJ::Matrix::NumberOfRows(tree());
}

int Matrix::numberOfColumns() const {
  return PoincareJ::Matrix::NumberOfColumns(tree());
}

// TODO_PCJ: Rework this and its usage
void Matrix::addChildAtIndexInPlace(JuniorExpression t, int index,
                                    int currentNumberOfChildren) {
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::EditionReference newChild = t.tree()->clone();
  if (index >= clone->numberOfChildren()) {
    int rows = PoincareJ::Matrix::NumberOfRows(clone);
    int columns = PoincareJ::Matrix::NumberOfColumns(clone);
    for (int i = 1; i < columns; i++) {
      PoincareJ::KUndef->clone();
    }
    PoincareJ::Matrix::SetNumberOfRows(clone, rows + 1);
  } else {
    PoincareJ::Tree* previousChild = clone->child(index);
    previousChild->removeTree();
    newChild->moveTreeAtNode(previousChild);
  }
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
}

JuniorExpression Matrix::matrixChild(int i, int j) {
  return JuniorExpression::Builder(PoincareJ::Matrix::Child(tree(), i, j));
}

int Matrix::rank(Context* context, bool forceCanonization) {
  if (!forceCanonization) {
    return PoincareJ::Matrix::Rank(tree());
  }
  PoincareJ::Tree* clone = tree()->clone();
  int result = PoincareJ::Matrix::CanonizeAndRank(clone);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
  return result;
}

// TODO_PCJ: Rework this and its usage
template <typename T>
int Matrix::ArrayInverse(T* array, int numberOfRows, int numberOfColumns) {
  return OMatrix::ArrayInverse(array, numberOfRows, numberOfColumns);
}

template int Matrix::ArrayInverse<double>(double*, int, int);
template int Matrix::ArrayInverse<std::complex<float>>(std::complex<float>*,
                                                       int, int);
template int Matrix::ArrayInverse<std::complex<double>>(std::complex<double>*,
                                                        int, int);

/* Point */

Point Point::Builder(JuniorExpression x, JuniorExpression y) {
  PoincareJ::Tree* tree = PoincareJ::KPoint->cloneNode();
  x.tree()->clone();
  y.tree()->clone();
  JuniorExpression temp = JuniorExpression::Builder(tree);
  return static_cast<Point&>(temp);
}

template <typename T>
Coordinate2D<T> Point::approximate2D(
    const ApproximationContext& approximationContext) {
  // TODO_PCJ: Add context for angle unit and complex format.
  return Coordinate2D<T>(
      PoincareJ::Approximation::RootTreeTo<T>(tree()->child(0)),
      PoincareJ::Approximation::RootTreeTo<T>(tree()->child(1)));
}

Layout Point::create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                             int significantDigits, Context* context) const {
  Layout child0 = childAtIndex(0).createLayout(floatDisplayMode,
                                               significantDigits, context);
  Layout child1 = childAtIndex(1).createLayout(floatDisplayMode,
                                               significantDigits, context);
  return JuniorLayout::Create(KPoint2DL(KA, KB), {.KA = child0, .KB = child1});
}

template Coordinate2D<float> Point::approximate2D<float>(
    const ApproximationContext& approximationContext);
template Coordinate2D<double> Point::approximate2D<double>(
    const ApproximationContext& approximationContext);

/* List */

List List::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::List>(0));
  return static_cast<List&>(expr);
}

// TODO_PCJ: Rework this and its usage
void List::addChildAtIndexInPlace(JuniorExpression t, int index,
                                  int currentNumberOfChildren) {
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::EditionReference newChild = t.tree()->clone();
  PoincareJ::NAry::SetNumberOfChildren(clone, clone->numberOfChildren() + 1);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<List&>(temp);
}

/* Unit */

JuniorExpression Unit::Builder(Preferences::AngleUnit angleUnit) {
  return JuniorExpression::Builder(PoincareJ::Units::Unit::Push(
      angleUnit == Preferences::AngleUnit::Radian
          ? &PoincareJ::Units::Angle::representatives.radian
      : angleUnit == Preferences::AngleUnit::Degree
          ? &PoincareJ::Units::Angle::representatives.degree
          : &PoincareJ::Units::Angle::representatives.gradian,
      PoincareJ::Units::Prefix::EmptyPrefix()));
}

bool Unit::IsPureAngleUnit(JuniorExpression expression, bool isRadian) {
  return PoincareJ::Units::IsPureAngleUnit(expression.tree()) &&
         (!isRadian ||
          PoincareJ::Units::Unit::GetRepresentative(expression.tree()) ==
              &PoincareJ::Units::Angle::representatives.radian);
}

bool Unit::HasAngleDimension(JuniorExpression expression) {
  assert(PoincareJ::Dimension::DeepCheckDimensions(expression.tree()));
  return PoincareJ::Dimension::GetDimension(expression.tree()).isAngleUnit();
}

}  // namespace Poincare
