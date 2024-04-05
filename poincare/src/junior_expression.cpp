#include <poincare/boolean.h>
#include <poincare/complex.h>
#include <poincare/junior_expression.h>
#include <poincare/junior_layout.h>
#include <poincare/k_tree.h>
#include <poincare/list_complex.h>
#include <poincare/matrix.h>
#include <poincare/matrix_complex.h>
#include <poincare/point_evaluation.h>
#include <poincare/symbol.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/continuity.h>
#include <poincare_junior/src/expression/conversion.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/unit.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/layout/serialize.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/tree.h>
#include <poincare_junior/src/n_ary.h>

namespace Poincare {

/* JuniorExpressionNode */

JuniorExpressionNode::JuniorExpressionNode(const PoincareJ::Tree* tree,
                                           size_t treeSize) {
  memcpy(m_blocks, tree->block(), treeSize);
}

size_t JuniorExpressionNode::size() const {
  return sizeof(JuniorExpressionNode) + tree()->treeSize();
}

#if POINCARE_TREE_LOG
void JuniorExpressionNode::logAttributes(std::ostream& stream) const {
  stream << '\n';
  tree()->log(stream);
}
#endif

int JuniorExpressionNode::simplificationOrderSameType(
    const ExpressionNode* e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(otype() == e->otype());
  return PoincareJ::Comparison::Compare(
      tree(), static_cast<const JuniorExpressionNode*>(e)->tree());
}

// Only handle approximated Boolean, Point and Complex trees.
template <typename T>
Evaluation<T> EvaluationFromSimpleTree(const PoincareJ::Tree* tree) {
  if (tree->isBoolean()) {
    return BooleanEvaluation<T>::Builder(
        PoincareJ::Approximation::ToBoolean<T>(tree));
  }
  if (tree->isPoint()) {
    assert(false);
    // TODO_PCJ: To implement.
    // return PointEvaluation<T>::Builder()
  }
  return Complex<T>::Builder(PoincareJ::Approximation::ToComplex<T>(tree));
}

// Return the Evaluation for any tree.
template <typename T>
Evaluation<T> EvaluationFromTree(
    const PoincareJ::Tree* origin,
    const ApproximationContext& approximationContext) {
  PoincareJ::Tree* tree = PoincareJ::Approximation::RootTreeToTree<T>(
      origin,
      static_cast<PoincareJ::AngleUnit>(approximationContext.angleUnit()),
      static_cast<PoincareJ::ComplexFormat>(
          approximationContext.complexFormat()));
  Evaluation<T> result;
  if (tree->isMatrix()) {
    MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
    int i = 0;
    for (const PoincareJ::Tree* child : tree->children()) {
      matrix.addChildAtIndexInPlace(EvaluationFromSimpleTree<T>(child), i, i);
      i++;
    }
    result = matrix;
  } else if (tree->isList()) {
    ListComplex<T> list = ListComplex<T>::Builder();
    int i = 0;
    for (const PoincareJ::Tree* child : tree->children()) {
      list.addChildAtIndexInPlace(EvaluationFromSimpleTree<T>(child), i, i);
      i++;
    }
    result = list;
  } else {
    result = EvaluationFromSimpleTree<T>(tree);
  }
  tree->removeTree();
  return result;
}

Evaluation<float> JuniorExpressionNode::approximate(
    SinglePrecision p, const ApproximationContext& approximationContext) const {
  return EvaluationFromTree<float>(tree(), approximationContext);
}

Evaluation<double> JuniorExpressionNode::approximate(
    DoublePrecision p, const ApproximationContext& approximationContext) const {
  return EvaluationFromTree<double>(tree(), approximationContext);
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
  PoincareJ::Tree* layout = PoincareJ::Layoutter::LayoutExpression(
      tree()->clone(), true, numberOfSignificantDigits);
  size_t size =
      PoincareJ::Serialize(layout, buffer, buffer + bufferSize) - buffer;
  layout->removeTree();
  return size;
}

bool JuniorExpressionNode::derivate(const ReductionContext& reductionContext,
                                    Symbol symbol, OExpression symbolValue) {
  // TODO PCJ: Remove
  assert(false);
  return false;
}

const PoincareJ::Tree* JuniorExpressionNode::tree() const {
  return PoincareJ::Tree::FromBlocks(m_blocks);
}

/* JuniorExpression */

JuniorExpression JuniorExpression::Parse(const PoincareJ::Tree* layout,
                                         Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  // TODO_PCJ: Use addMissingParenthesis and parseForAssignment.
  return Builder(PoincareJ::Parser::Parse(layout, context));
}

JuniorExpression JuniorExpression::Parse(const char* string, Context* context,
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

JuniorExpression JuniorExpression::Create(const PoincareJ::Tree* structure,
                                          PoincareJ::ContextTrees ctx) {
  PoincareJ::Tree* tree = PoincareJ::PatternMatching::Create(structure, ctx);
  return Builder(tree);
}

// Builders from value.
JuniorExpression JuniorExpression::Builder(int32_t n) {
  return Builder(PoincareJ::Integer::Push(n));
}

template <>
JuniorExpression JuniorExpression::Builder<float>(float x) {
  return Builder(
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::SingleFloat>(x));
}

template <>
JuniorExpression JuniorExpression::Builder<double>(double x) {
  return Builder(
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::DoubleFloat>(x));
}

JuniorExpression JuniorExpression::Builder(const PoincareJ::Tree* tree) {
  if (!tree) {
    return JuniorExpression();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      Pool::sharedPool->alloc(sizeof(JuniorExpressionNode) + size);
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

int JuniorExpression::numberOfDescendants(bool includeSelf) const {
  assert(tree());
  return tree()->numberOfDescendants(includeSelf);
}

ExpressionNode::Type JuniorExpression::type() const {
  /* TODO_PCJ: These are the types checked for in apps. Update apps to use the
   *           new blockType instead. */
  if (tree()->isRational()) {
    return tree()->isInteger() ? ExpressionNode::Type::BasedInteger
                               : ExpressionNode::Type::Rational;
  }
  switch (tree()->type()) {
    case PoincareJ::Type::Addition:
      return ExpressionNode::Type::Addition;
    case PoincareJ::Type::True:
    case PoincareJ::Type::False:
      return ExpressionNode::Type::Boolean;
    case PoincareJ::Type::ComplexArgument:
      return ExpressionNode::Type::ComplexArgument;
    case PoincareJ::Type::Conjugate:
      return ExpressionNode::Type::Conjugate;
    case PoincareJ::Type::PhysicalConstant:
      return ExpressionNode::Type::ConstantPhysics;
    case PoincareJ::Type::Dependency:
      return ExpressionNode::Type::Dependency;
    case PoincareJ::Type::Derivative:
      return ExpressionNode::Type::Derivative;
    case PoincareJ::Type::Division:
      return ExpressionNode::Type::Division;
    case PoincareJ::Type::Factor:
      return ExpressionNode::Type::Factor;
    case PoincareJ::Type::FracPart:
      return ExpressionNode::Type::FracPart;
    case PoincareJ::Type::ImaginaryPart:
      return ExpressionNode::Type::ImaginaryPart;
    case PoincareJ::Type::Infinity:
      return ExpressionNode::Type::Infinity;
    case PoincareJ::Type::Integral:
      return ExpressionNode::Type::Integral;
    case PoincareJ::Type::List:
      return ExpressionNode::Type::List;
    case PoincareJ::Type::ListSequence:
      return ExpressionNode::Type::ListSequence;
    case PoincareJ::Type::Logarithm:
    case PoincareJ::Type::Log:
      return ExpressionNode::Type::Logarithm;
    case PoincareJ::Type::Matrix:
      return ExpressionNode::Type::Matrix;
    case PoincareJ::Type::Multiplication:
      return ExpressionNode::Type::Multiplication;
    case PoincareJ::Type::Nonreal:
      return ExpressionNode::Type::Nonreal;
    case PoincareJ::Type::Opposite:
      return ExpressionNode::Type::Opposite;
    case PoincareJ::Type::Piecewise:
      return ExpressionNode::Type::PiecewiseOperator;
    case PoincareJ::Type::Point:
      return ExpressionNode::Type::Point;
    case PoincareJ::Type::Power:
      return ExpressionNode::Type::Power;
    case PoincareJ::Type::Product:
      return ExpressionNode::Type::Product;
    case PoincareJ::Type::RandInt:
      return ExpressionNode::Type::Randint;
    case PoincareJ::Type::RandIntNoRep:
      return ExpressionNode::Type::RandintNoRepeat;
    case PoincareJ::Type::Random:
      return ExpressionNode::Type::Random;
    case PoincareJ::Type::RealPart:
      return ExpressionNode::Type::RealPart;
    case PoincareJ::Type::Round:
      return ExpressionNode::Type::Round;
    case PoincareJ::Type::Store:
      return ExpressionNode::Type::Store;
    case PoincareJ::Type::Sum:
      return ExpressionNode::Type::Sum;
    case PoincareJ::Type::Undefined:
      return ExpressionNode::Type::Undefined;
    case PoincareJ::Type::UnitConversion:
      return ExpressionNode::Type::UnitConvert;
    case PoincareJ::Type::UserSymbol:
      return ExpressionNode::Type::Symbol;
    case PoincareJ::Type::UserFunction:
      return ExpressionNode::Type::Function;
    case PoincareJ::Type::UserSequence:
      return ExpressionNode::Type::Sequence;
    case PoincareJ::Type::Parenthesis:
      return ExpressionNode::Type::Parenthesis;
#if 0
      // No perfect PoincareJ equivalents
      return ExpressionNode::Type::Comparison;
      return ExpressionNode::Type::ConstantMaths;
      return ExpressionNode::Type::DistributionDispatcher;
#endif
      // Unused in apps, but they should not raise the default assert.
    case PoincareJ::Type::Equal:
    case PoincareJ::Type::NotEqual:
    case PoincareJ::Type::Superior:
    case PoincareJ::Type::SuperiorEqual:
    case PoincareJ::Type::Inferior:
    case PoincareJ::Type::InferiorEqual:
      // TODO_PCJ
      return ExpressionNode::Type::Comparison;
    default:
      return ExpressionNode::Type::JuniorExpression;
  }
}

bool JuniorExpression::isOfType(
    std::initializer_list<ExpressionNode::Type> types) const {
  for (ExpressionNode::Type t : types) {
    if (type() == t) {
      return true;
    }
  }
  return false;
}

void JuniorExpression::cloneAndSimplifyAndApproximate(
    JuniorExpression* simplifiedExpression,
    JuniorExpression* approximateExpression,
    const ReductionContext& reductionContext,
    bool approximateKeepingSymbols) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximateExpression || approximateExpression->isUninitialized());
  assert(reductionContext.target() == ReductionTarget::User);
  PoincareJ::ProjectionContext context = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_strategy = approximateKeepingSymbols
                        ? PoincareJ::Strategy::ApproximateToFloat
                        : PoincareJ::Strategy::Default,
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  PoincareJ::Tree* e = tree()->clone();
  PoincareJ::Simplification::Simplify(e, &context);
  if (approximateExpression) {
    *approximateExpression =
        Builder(PoincareJ::Approximation::RootTreeToTree<double>(
            e, context.m_angleUnit, context.m_complexFormat));
  }
  *simplifiedExpression = Builder(e);
  return;
}

JuniorExpression JuniorExpression::cloneAndDeepReduceWithSystemCheckpoint(
    ReductionContext* reductionContext, bool* reduceFailure,
    bool approximateDuringReduction) const {
  PoincareJ::Strategy initialStrategy =
      approximateDuringReduction ? PoincareJ::Strategy::ApproximateToFloat
                                 : PoincareJ::Strategy::Default;
  PoincareJ::ProjectionContext context = {
    .m_complexFormat = reductionContext->complexFormat(),
    .m_angleUnit = reductionContext->angleUnit(),
#if 1
    .m_strategy = initialStrategy,
#endif
    .m_unitFormat = reductionContext->unitFormat(),
    .m_symbolic = reductionContext->symbolicComputation(),
    .m_context = reductionContext->context()
  };
  PoincareJ::Tree* e = tree()->clone();
  // TODO_PCJ: Do not beautify !! Decide if a projection is needed.
  PoincareJ::Simplification::Simplify(e, &context);
  *reduceFailure = context.m_strategy != initialStrategy;
  JuniorExpression simplifiedExpression = Builder(e);
#if 0
  if (approximateDuringReduction) {
    /* TODO_PCJ: We used to approximate after a full reduction (see comment).
     *           Ensure this is no longer necessary. */
    /* It is always needed to reduce when approximating keeping symbols to
     * catch reduction failure and abort if necessary.
     *
     * The expression is reduced before and not during approximation keeping
     * symbols even because deepApproximateKeepingSymbols can only partially
     * reduce the expression.
     *
     * For example, if e="x*x+x^2":
     * "x*x" will be reduced to "x^rational(2)", while "x^2" will be
     * reduced/approximated to "x^float(2.)".
     * Then "x^rational(2)+x^float(2.)" won't be able to reduce to
     * "2*x^float(2.)" because float(2.) != rational(2.).
     * This does not happen if e is reduced beforehand. */
    simplifiedExpression =
        simplifiedExpression.deepApproximateKeepingSymbols(*reductionContext);
  }
  if (!*reduceFailure) {
    /* TODO_PCJ: Ensure Dependency::deepRemoveUselessDependencies(...) logic has
     * been properly brought in PoincareJ::Simplification. */
    simplifiedExpression =
        simplifiedExpression.deepRemoveUselessDependencies(*reductionContext);
  }
#endif
  assert(!simplifiedExpression.isUninitialized());
  return simplifiedExpression;
}

JuniorExpression JuniorExpression::cloneAndReduce(
    ReductionContext reductionContext) const {
  // TODO: Ensure all cloneAndReduce usages handle reduction failure.
  bool reduceFailure;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext,
                                                &reduceFailure);
}

JuniorExpression JuniorExpression::cloneAndApproximateKeepingSymbols(
    ReductionContext reductionContext) const {
  bool dummy;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &dummy,
                                                true);
}

JuniorExpression JuniorExpression::cloneAndSimplify(
    ReductionContext reductionContext, bool* reductionFailure) const {
  bool reduceFailure = false;
  JuniorExpression e =
      cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reductionFailure) {
    *reductionFailure = reduceFailure;
  }
#if 0  // TODO_PCJ
  if (reduceFailure ||
      (otype() == ExpressionNode::Type::Store &&
       !static_cast<const Store*>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    return e;
  }
#else
  assert(!reduceFailure && (type() != ExpressionNode::Type::Store));
#endif
  /* TODO_PCJ: Beautify since cloneAndDeepReduceWithSystemCheckpoint isn't
   * supposed to. */
  return e.deepBeautify(reductionContext);
}

bool JuniorExpression::derivate(const ReductionContext& reductionContext,
                                Symbol symbol, OExpression symbolValue) {
  // TODO PCJ: Remove
  assert(false);
  return false;
}

int JuniorExpression::getPolynomialCoefficients(
    Context* context, const char* symbolName,
    JuniorExpression coefficients[]) const {
  PoincareJ::Tree* symbol =
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::UserSymbol>(symbolName);
  PoincareJ::Tree* poly =
      PoincareJ::PolynomialParser::Parse(tree()->clone(), symbol);
  int degree = poly->isPolynomial() ? PoincareJ::Polynomial::Degree(poly) : 0;
  int indexExponent = 0;
  int numberOfTerms = PoincareJ::Polynomial::NumberOfTerms(poly);
  for (int i = degree; i >= 0; i--) {
    if (indexExponent < numberOfTerms &&
        i == PoincareJ::Polynomial::ExponentAtIndex(poly, indexExponent)) {
      coefficients[i] = Builder(poly->child(indexExponent + 1)->clone());
      indexExponent++;
    } else {
      coefficients[i] =
          Builder(PoincareJ::SharedTreeStack->push(PoincareJ::Type::Zero));
    }
  }
  assert(indexExponent == PoincareJ::Polynomial::NumberOfTerms(poly));
  poly->removeTree();
  symbol->removeTree();
  return degree;
}

int JuniorExpression::getPolynomialReducedCoefficients(
    const char* symbolName, JuniorExpression coefficients[], Context* context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation,
    bool keepDependencies) const {
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].cloneAndReduce(ReductionContext(
        context, complexFormat, angleUnit, unitFormat,
        ReductionTarget::SystemForApproximation, symbolicComputation));
    if (!keepDependencies &&
        coefficients[i].type() == ExpressionNode::Type::Dependency) {
      coefficients[i] = coefficients[i].childAtIndex(0);
    }
  }
  return degree;
}

int JuniorExpression::polynomialDegree(Context* context,
                                       const char* symbolName) const {
  PoincareJ::Tree* symbol =
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::UserSymbol>(symbolName);
  PoincareJ::Tree* poly =
      PoincareJ::PolynomialParser::Parse(tree()->clone(), symbol);
  int degree = poly->isPolynomial() ? PoincareJ::Polynomial::Degree(poly) : 0;
  poly->removeTree();
  symbol->removeTree();
  return degree;
}

JuniorExpression JuniorExpression::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const JuniorExpression& expression) {
  // TODO_PCJ: Ensure symbol is variable and use PoincareJ::Variables::Replace
  /* TODO_PCJ: Handle parametrics, functions and sequences as well. See
   * replaceSymbolWithExpression implementations. */
  if (isUninitialized()) {
    return *this;
  }
  assert(symbol.tree()->isUserSymbol() &&
         !tree()->hasDescendantSatisfying(
             [](const PoincareJ::Tree* e) { return e->isParametric(); }));
  PoincareJ::Tree* result = tree()->clone();
  if (result->deepReplaceWith(symbol.tree(), expression.tree())) {
    JuniorExpression res = Builder(result);
    replaceWithInPlace(res);
    return res;
  }
  result->removeTree();
  return *this;
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
  if (t == ExpressionNode::Type::Dependency ||
      t == ExpressionNode::Type::Store) {
    return childAtIndex(0).recursivelyMatches(test, context, replaceSymbols,
                                              auxiliary, ignoredSymbols);
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
    return SymbolAbstract::matches(convert<const SymbolAbstract>(), test,
                                   context, auxiliary, ignoredSymbols);
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

bool JuniorExpression::deepIsOfType(
    std::initializer_list<ExpressionNode::Type> types, Context* context) const {
  return recursivelyMatches(
      [](const Expression e, Context* context, void* auxiliary) {
        return e.isOfType(
                   *static_cast<std::initializer_list<ExpressionNode::Type>*>(
                       auxiliary))
                   ? TrinaryBoolean::True
                   : TrinaryBoolean::Unknown;
      },
      context, SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      &types);
}

bool JuniorExpression::deepIsMatrix(Context* context, bool canContainMatrices,
                                    bool isReduced) const {
  if (!canContainMatrices) {
    return false;
  }
  return PoincareJ::Dimension::GetDimension(tree()).isMatrix();
}

bool JuniorExpression::deepIsList(Context* context) const {
  return PoincareJ::Dimension::GetListLength(tree()) >= 0;
}

bool JuniorExpression::hasComplexI(Context* context,
                                   SymbolicComputation replaceSymbols) const {
  return !isUninitialized() &&
         recursivelyMatches(
             [](const JuniorExpression e, Context* context) {
               return e.tree()->isComplexI();
             },
             context, replaceSymbols);
}

bool JuniorExpression::hasUnit(bool ignoreAngleUnits, bool* hasAngleUnits,
                               bool replaceSymbols, Context* ctx) const {
  if (hasAngleUnits) {
    *hasAngleUnits = false;
  }
  struct Pack {
    bool ignoreAngleUnits;
    bool* hasAngleUnits;
  };
  Pack pack{ignoreAngleUnits, hasAngleUnits};
  return recursivelyMatches(
      [](const JuniorExpression e, Context* context, void* arg) {
        Pack* pack = static_cast<Pack*>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool* hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.type() == ExpressionNode::Type::Unit &&
                (!pack->ignoreAngleUnits || !isAngleUnit)) ||
               e.type() == ExpressionNode::Type::ConstantPhysics;
      },
      ctx,
      replaceSymbols
          ? SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
          : SymbolicComputation::DoNotReplaceAnySymbol,
      &pack);
}

bool JuniorExpression::isPureAngleUnit() const {
  return !isUninitialized() && type() == ExpressionNode::Type::Unit &&
         PoincareJ::Dimension::GetDimension(tree()).isSimpleAngleUnit();
}

bool JuniorExpression::isInRadians(Context* context) const {
  JuniorExpression units;
  ReductionContext reductionContext;
  reductionContext.setContext(context);
  reductionContext.setUnitConversion(UnitConversion::None);
  JuniorExpression thisClone =
      cloneAndReduceAndRemoveUnit(reductionContext, &units);
  return !units.isUninitialized() &&
         units.type() == ExpressionNode::Type::Unit &&
         PoincareJ::Dimension::GetDimension(tree()).isSimpleRadianAngleUnit();
}

bool JuniorExpression::involvesDiscontinuousFunction(Context* context) const {
  return recursivelyMatches(IsDiscontinuous, context);
}

bool JuniorExpression::IsDiscontinuous(const JuniorExpression e,
                                       Context* context) {
  return PoincareJ::Continuity::InvolvesDiscontinuousFunction(e.tree());
}

/* Matrix */

Matrix Matrix::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::Matrix>(0, 0));
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
  PoincareJ::TreeRef newChild = t.tree()->clone();
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
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::List>(0));
  return static_cast<List&>(expr);
}

// TODO_PCJ: Rework this and its usage
void List::addChildAtIndexInPlace(JuniorExpression t, int index,
                                  int currentNumberOfChildren) {
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::TreeRef newChild = t.tree()->clone();
  PoincareJ::NAry::SetNumberOfChildren(clone, clone->numberOfChildren() + 1);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<List&>(temp);
}

/* Boolean */

bool Boolean::value() const {
  assert(tree()->isTrue() || tree()->isFalse());
  return tree()->isTrue();
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

template Evaluation<float> EvaluationFromTree<float>(
    const PoincareJ::Tree*, const ApproximationContext&);
template Evaluation<double> EvaluationFromTree<double>(
    const PoincareJ::Tree*, const ApproximationContext&);

template Evaluation<float> EvaluationFromSimpleTree<float>(
    const PoincareJ::Tree*);
template Evaluation<double> EvaluationFromSimpleTree<double>(
    const PoincareJ::Tree*);

}  // namespace Poincare
