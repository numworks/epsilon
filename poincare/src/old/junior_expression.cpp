#include <poincare/cas.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/old/boolean.h>
#include <poincare/old/complex.h>
#include <poincare/old/junior_expression.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/point_evaluation.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/continuity.h>
#include <poincare/src/expression/conversion.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/order.h>
#include <poincare/src/expression/parametric.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/representatives.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/latex_parser.h>
#include <poincare/src/layout/parsing/parsing_context.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include <complex>

namespace Poincare {

using namespace Internal;

/* Dimension */

Poincare::Dimension::Dimension(const NewExpression e, Context* context)
    : m_matrixDimension({.rows = 0, .cols = 0}),
      m_type(DimensionType::Scalar),
      m_isList(false),
      m_isValid(false) {
  // TODO_PCJ: Remove checks in ProjectedExpression implementation of this
  if (!Internal::Dimension::DeepCheck(e.tree(), context)) {
    return;
  }
  Internal::Dimension dimension = Internal::Dimension::Get(e.tree(), context);
  m_type = dimension.type;
  m_isList = Internal::Dimension::IsList(e.tree(), context);
  if (m_type == DimensionType::Matrix) {
    m_matrixDimension = dimension.matrix;
  }
  m_isValid = true;
}

bool Poincare::Dimension::isScalar() {
  return m_isValid && !m_isList && m_type == DimensionType::Scalar;
}

bool Poincare::Dimension::isMatrix() {
  return m_isValid && !m_isList && m_type == DimensionType::Matrix;
}

bool Poincare::Dimension::isVector() {
  return isMatrix() &&
         (m_matrixDimension.rows == 1 || m_matrixDimension.cols == 1);
}

bool Poincare::Dimension::isUnit() {
  return m_isValid && !m_isList && m_type == DimensionType::Unit;
}

bool Poincare::Dimension::isBoolean() {
  return m_isValid && !m_isList && m_type == DimensionType::Boolean;
}

bool Poincare::Dimension::isPoint() {
  /* TODO_PCJ: This method used to allow (undef, x) with x undefined. Restore
   * this behavior ? */
  return m_isValid && !m_isList && m_type == DimensionType::Point;
}

bool Poincare::Dimension::isList() { return m_isValid && m_isList; }

bool Poincare::Dimension::isListOfScalars() {
  return m_isValid && m_isList && m_type == DimensionType::Scalar;
}

bool Poincare::Dimension::isListOfUnits() {
  return m_isValid && m_isList && m_type == DimensionType::Unit;
}

bool Poincare::Dimension::isListOfBooleans() {
  return m_isValid && m_isList && m_type == DimensionType::Boolean;
}

bool Poincare::Dimension::isListOfPoints() {
  return m_isValid && m_isList && m_type == DimensionType::Point;
}

bool Poincare::Dimension::isPointOrListOfPoints() {
  return m_isValid && m_type == DimensionType::Point;
}

/* JuniorExpressionNode */

JuniorExpressionNode::JuniorExpressionNode(const Tree* tree, size_t treeSize) {
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
  return Order::CompareSystem(
      tree(), static_cast<const JuniorExpressionNode*>(e)->tree());
}

// Only handle approximated Boolean, Point and Complex trees.
template <typename T>
Evaluation<T> EvaluationFromSimpleTree(const Tree* tree) {
  if (tree->isBoolean()) {
    return BooleanEvaluation<T>::Builder(Approximation::ToBoolean<T>(tree));
  }
  if (tree->isPoint()) {
    assert(false);
    // TODO_PCJ: To implement.
    // return PointEvaluation<T>::Builder()
  }
  return Complex<T>::Builder(Approximation::ToComplex<T>(tree));
}

// Return the Evaluation for any tree.
template <typename T>
Evaluation<T> EvaluationFromTree(
    const Tree* origin, const ApproximationContext& approximationContext) {
  Tree* tree = Approximation::RootTreeToTree<T>(
      origin, static_cast<AngleUnit>(approximationContext.angleUnit()),
      static_cast<ComplexFormat>(approximationContext.complexFormat()));
  Evaluation<T> result;
  if (tree->isMatrix()) {
    MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
    int i = 0;
    for (const Tree* child : tree->children()) {
      matrix.addChildAtIndexInPlace(EvaluationFromSimpleTree<T>(child), i, i);
      i++;
    }
    result = matrix;
  } else if (tree->isList()) {
    ListComplex<T> list = ListComplex<T>::Builder();
    int i = 0;
    for (const Tree* child : tree->children()) {
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

template <typename T>
SystemExpression JuniorExpressionNode::approximateToTree(
    const ApproximationContext& approximationContext) const {
  return SystemExpression::Builder(Approximation::RootTreeToTree<T>(
      tree(), static_cast<AngleUnit>(approximationContext.angleUnit()),
      static_cast<ComplexFormat>(approximationContext.complexFormat())));
}

Poincare::Layout JuniorExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context, OMG::Base base) const {
  return Poincare::Layout::Builder(Layouter::LayoutExpression(
      tree()->cloneTree(), false, numberOfSignificantDigits, floatDisplayMode,
      base));
}

size_t JuniorExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  Tree* layout = Layouter::LayoutExpression(tree()->cloneTree(), true,
                                            numberOfSignificantDigits);
  size_t size = Serialize(layout, buffer, buffer + bufferSize) - buffer;
  layout->removeTree();
  return size;
}

bool JuniorExpressionNode::derivate(const ReductionContext& reductionContext,
                                    Poincare::Symbol symbol,
                                    OExpression symbolValue) {
  // TODO_PCJ: Remove
  assert(false);
  return false;
}

const Tree* JuniorExpressionNode::tree() const {
  return Tree::FromBlocks(m_blocks);
}

/* JuniorExpression */

UserExpression UserExpression::Parse(const Tree* layout, Context* context,
                                     bool addMissingParenthesis,
                                     bool parseForAssignment) {
  // TODO_PCJ: Use addMissingParenthesis
  return Builder(Parser::Parse(layout, context,
                               parseForAssignment
                                   ? ParsingContext::ParsingMethod::Assignment
                                   : ParsingContext::ParsingMethod::Classic));
}

UserExpression UserExpression::Parse(const char* string, Context* context,
                                     bool addMissingParenthesis,
                                     bool parseForAssignment) {
  if (string[0] == 0) {
    return UserExpression();
  }
  Tree* layout = RackFromText(string);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result =
      Parse(layout, context, addMissingParenthesis, parseForAssignment);
  layout->removeTree();
  return result;
}

UserExpression UserExpression::ParseLatex(const char* latex, Context* context,
                                          bool addMissingParenthesis,
                                          bool parseForAssignment) {
  Tree* layout = LatexParser::LatexToLayout(latex);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result =
      Parse(layout, context, addMissingParenthesis, parseForAssignment);
  layout->removeTree();
  return result;
}

NewExpression NewExpression::Create(const Tree* structure, ContextTrees ctx) {
  Tree* tree = PatternMatching::Create(structure, ctx);
  return Builder(tree);
}

SystemExpression SystemExpression::CreateReduce(const Tree* structure,
                                                ContextTrees ctx) {
  Tree* tree = PatternMatching::CreateSimplify(structure, ctx);
  return Builder(tree);
}

// Builders from value.
SystemExpression SystemExpression::Builder(int32_t n) {
  return Builder(Integer::Push(n));
}

template <typename T>
SystemExpression SystemExpression::Builder(T x) {
  return Builder(SharedTreeStack->pushFloat(x));
}

template <typename T>
SystemExpression SystemExpression::Builder(Coordinate2D<T> point) {
  return Create(KPoint(KA, KB),
                {.KA = Builder<T>(point.x()), .KB = Builder<T>(point.y())});
}

template <typename T>
SystemExpression SystemExpression::Builder(PointOrScalar<T> pointOrScalar) {
  if (pointOrScalar.isScalar()) {
    return Builder<T>(pointOrScalar.toScalar());
  }
  return Builder<T>(pointOrScalar.toPoint());
}

SystemExpression SystemExpression::DecimalBuilderFromDouble(double value) {
  // TODO: this is a workaround until we port old Decimal::Builder(double)
  char buffer[PrintFloat::k_maxFloatCharSize];
  PrintFloat::PrintFloat::ConvertFloatToText(
      value, buffer, PrintFloat::k_maxFloatCharSize,
      PrintFloat::k_maxFloatGlyphLength,
      PrintFloat::k_maxNumberOfSignificantDigits,
      Preferences::PrintFloatMode::Decimal);
  return UserExpression::Parse(buffer, nullptr);
}

NewExpression NewExpression::Builder(const Tree* tree) {
  if (!tree) {
    return NewExpression();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      Pool::sharedPool->alloc(sizeof(JuniorExpressionNode) + size);
  JuniorExpressionNode* node =
      new (bufferNode) JuniorExpressionNode(tree, size);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<NewExpression&>(h);
}

NewExpression NewExpression::Builder(Tree* tree) {
  NewExpression result = Builder(const_cast<const Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

NewExpression NewExpression::Juniorize(OExpression e) {
  if (e.isUninitialized() ||
      e.otype() == ExpressionNode::Type::JuniorExpression) {
    // e is already a junior expression
    return static_cast<NewExpression&>(e);
  }
  return Builder(FromPoincareExpression(e));
}

NewExpression NewExpression::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

int NewExpression::numberOfDescendants(bool includeSelf) const {
  assert(tree());
  return tree()->numberOfDescendants(includeSelf);
}

ExpressionNode::Type NewExpression::type() const {
  /* TODO_PCJ: These are the types checked for in apps. Update apps to use the
   *           new type instead. */
  if (tree()->isRational()) {
    return tree()->isInteger() ? ExpressionNode::Type::BasedInteger
                               : ExpressionNode::Type::Rational;
  }
  if (tree()->isUndefined()) {
    return tree()->isNonReal() ? ExpressionNode::Type::Nonreal
                               : ExpressionNode::Type::Undefined;
  }
  switch (tree()->type()) {
    case Type::Add:
      return ExpressionNode::Type::Addition;
    case Type::True:
    case Type::False:
      return ExpressionNode::Type::Boolean;
    case Type::Arg:
      return ExpressionNode::Type::ComplexArgument;
    case Type::Conj:
      return ExpressionNode::Type::Conjugate;
    case Type::PhysicalConstant:
      return ExpressionNode::Type::ConstantPhysics;
    case Type::Dep:
      return ExpressionNode::Type::Dependency;
    case Type::Div:
      return ExpressionNode::Type::Division;
    case Type::Factor:
      return ExpressionNode::Type::Factor;
    case Type::Frac:
      return ExpressionNode::Type::FracPart;
    case Type::Im:
      return ExpressionNode::Type::ImaginaryPart;
    case Type::Inf:
      return ExpressionNode::Type::Infinity;
    case Type::Integral:
      return ExpressionNode::Type::Integral;
    case Type::List:
      return ExpressionNode::Type::List;
    case Type::ListSequence:
      return ExpressionNode::Type::ListSequence;
    case Type::LogBase:
    case Type::Log:
      return ExpressionNode::Type::Logarithm;
    case Type::Matrix:
      return ExpressionNode::Type::Matrix;
    case Type::Mult:
      return ExpressionNode::Type::Multiplication;
    case Type::Opposite:
      return ExpressionNode::Type::Opposite;
    case Type::Piecewise:
      return ExpressionNode::Type::PiecewiseOperator;
    case Type::Point:
      return ExpressionNode::Type::Point;
    case Type::Pow:
      return ExpressionNode::Type::Power;
    case Type::Product:
      return ExpressionNode::Type::Product;
    case Type::RandInt:
      return ExpressionNode::Type::Randint;
    case Type::RandIntNoRep:
      return ExpressionNode::Type::RandintNoRepeat;
    case Type::Random:
      return ExpressionNode::Type::Random;
    case Type::Re:
      return ExpressionNode::Type::RealPart;
    case Type::Round:
      return ExpressionNode::Type::Round;
    case Type::Store:
      return ExpressionNode::Type::Store;
    case Type::Sum:
      return ExpressionNode::Type::Sum;
    case Type::UnitConversion:
      return ExpressionNode::Type::UnitConvert;
    case Type::UserSymbol:
      return ExpressionNode::Type::Symbol;
    case Type::UserFunction:
      return ExpressionNode::Type::Function;
    case Type::UserSequence:
      return ExpressionNode::Type::Sequence;
    case Type::Parentheses:
      return ExpressionNode::Type::Parenthesis;
    case Type::Cos:
      return ExpressionNode::Type::Cosine;
    case Type::Sin:
      return ExpressionNode::Type::Sine;
    case Type::Tan:
      return ExpressionNode::Type::Tangent;
    case Type::ACos:
      return ExpressionNode::Type::ArcCosine;
    case Type::ASin:
      return ExpressionNode::Type::ArcSine;
    case Type::ATan:
      return ExpressionNode::Type::ArcTangent;
    case Type::Sec:
      return ExpressionNode::Type::Secant;
    case Type::Csc:
      return ExpressionNode::Type::Cosecant;
    case Type::Cot:
      return ExpressionNode::Type::Cotangent;
    case Type::ASec:
      return ExpressionNode::Type::ArcSecant;
    case Type::ACsc:
      return ExpressionNode::Type::ArcCosecant;
    case Type::ACot:
      return ExpressionNode::Type::ArcCotangent;
    case Type::Unit:
      return ExpressionNode::Type::Unit;
    case Type::Diff:
      return ExpressionNode::Type::Derivative;
#if 0
      // No perfect Internal equivalents
      return ExpressionNode::Type::Comparison;
      return ExpressionNode::Type::ConstantMaths;
      return ExpressionNode::Type::DistributionDispatcher;
#endif
      // Unused in apps, but they should not raise the default assert.
    case Type::Equal:
    case Type::NotEqual:
    case Type::Superior:
    case Type::SuperiorEqual:
    case Type::Inferior:
    case Type::InferiorEqual:
      // TODO_PCJ
      return ExpressionNode::Type::Comparison;
    case Type::PercentSimple:
      return ExpressionNode::Type::PercentSimple;
    case Type::PercentAddition:
      return ExpressionNode::Type::PercentAddition;
    default:
      return ExpressionNode::Type::JuniorExpression;
  }
}

bool NewExpression::isOfType(
    std::initializer_list<Internal::Type> types) const {
  return tree()->isOfType(types);
}

bool NewExpression::deepIsOfType(std::initializer_list<Internal::Type> types,
                                 Context* context) const {
  return recursivelyMatches(
      [](const Expression e, Context* context, void* auxiliary) {
        return e.isOfType(*static_cast<std::initializer_list<Internal::Type>*>(
                   auxiliary))
                   ? OMG::Troolean::True
                   : OMG::Troolean::Unknown;
      },
      context, SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      &types);
}

void UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression,
    const ReductionContext& reductionContext,
    bool approximateKeepingSymbols) const {
  assert(reductionContext.target() == ReductionTarget::User);
  ProjectionContext context = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_strategy = approximateKeepingSymbols ? Strategy::ApproximateToFloat
                                              : Strategy::Default,
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  return cloneAndSimplifyAndApproximate(simplifiedExpression,
                                        approximatedExpression, &context);
}

void UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression,
    Internal::ProjectionContext* context) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximatedExpression || approximatedExpression->isUninitialized());
  Tree* e = tree()->cloneTree();
  Simplification::SimplifyWithAdaptiveStrategy(e, context);
  if (approximatedExpression) {
    if (CAS::Enabled()) {
      Tree* a = e->cloneTree();
      /* We are using ApproximateAndReplaceEveryScalar to approximate
       * expressions with symbols such as π*x → 3.14*x. */
      Approximation::ApproximateAndReplaceEveryScalar(a, context);
      *approximatedExpression = Builder(a);
    } else {
      *approximatedExpression = Builder(Approximation::RootTreeToTree<double>(
          e, context->m_angleUnit, context->m_complexFormat));
    }
  }
  *simplifiedExpression = Builder(e);
  return;
}

SystemExpression UserExpression::cloneAndDeepReduceWithSystemCheckpoint(
    ReductionContext* reductionContext, bool* reduceFailure,
    bool approximateDuringReduction) const {
  ProjectionContext context = {
      .m_complexFormat = reductionContext->complexFormat(),
      .m_angleUnit = reductionContext->angleUnit(),
      .m_strategy = approximateDuringReduction ? Strategy::ApproximateToFloat
                                               : Strategy::Default,
      .m_unitFormat = reductionContext->unitFormat(),
      .m_symbolic = reductionContext->symbolicComputation(),
      .m_context = reductionContext->context()};
  Tree* e = tree()->cloneTree();
  // TODO_PCJ: Decide if a projection is needed or not
  Simplification::ProjectAndReduceWithAdaptiveStrategy(e, &context, true);
  *reduceFailure = false;
  SystemExpression simplifiedExpression = Builder(e);
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
     * been properly brought in Simplification. */
    simplifiedExpression =
        simplifiedExpression.deepRemoveUselessDependencies(*reductionContext);
  }
#endif
  assert(!simplifiedExpression.isUninitialized());
  return simplifiedExpression;
}

SystemExpression UserExpression::cloneAndReduce(
    ReductionContext reductionContext) const {
  // TODO: Ensure all cloneAndReduce usages handle reduction failure.
  bool reduceFailure;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext,
                                                &reduceFailure);
}

SystemExpression SystemExpression::getReducedDerivative(
    const char* symbolName, int derivationOrder) const {
  Tree* result = SharedTreeStack->pushDiff();
  SharedTreeStack->pushUserSymbol(symbolName);
  const Tree* symbol = SharedTreeStack->pushUserSymbol(symbolName);
  Integer::Push(derivationOrder);
  Tree* derivand = tree()->cloneTree();
  Variables::ReplaceSymbol(derivand, symbol, 0,
                           Parametric::VariableSign(result));
  Simplification::ReduceSystem(result, false);
  /* TODO_PCJ: Derivative used to be simplified with SystemForApproximation, but
   * getSystemFunction is expected to be called on it later. */
  return SystemExpression::Builder(result);
}

SystemFunction SystemExpression::getSystemFunction(const char* symbolName,
                                                   bool scalarsOnly) const {
  Tree* result = tree()->cloneTree();
  Internal::Dimension dimension = Internal::Dimension::Get(tree());
  if ((scalarsOnly &&
       (!dimension.isScalar() || Internal::Dimension::IsList(tree()))) ||
      (!dimension.isScalar() && !dimension.isPoint())) {
    result->cloneTreeOverTree(KUndef);
  } else {
    Approximation::PrepareFunctionForApproximation(result, symbolName,
                                                   ComplexFormat::Real);
  }
  return JuniorExpression::Builder(result);
}

template <typename T>
T UserExpression::approximateUserExpressionToScalar(
    AngleUnit angleUnit, ComplexFormat complexFormat) const {
  return Approximation::RootTreeToReal<T>(tree(), angleUnit, complexFormat);
}

/* TODO_PCJ: This cannot be called on system expressions, but rather on
 * ScalarSystemFunction */
template <typename T>
T SystemExpression::approximateToScalarWithValue(T x, int listElement) const {
  return Approximation::RootPreparedToReal<T>(tree(), x, listElement);
}

template <typename T>
T UserExpression::ParseAndSimplifyAndApproximateToScalar(
    const char* text, Context* context,
    SymbolicComputation symbolicComputation) {
  UserExpression exp = ParseAndSimplify(text, context, symbolicComputation);
  assert(!exp.isUninitialized());
  return exp.approximateToScalar<T>(context);
}

template <typename T>
bool UserExpression::hasDefinedComplexApproximation(
    const ApproximationContext& approximationContext, T* returnRealPart,
    T* returnImagPart) const {
  if (approximationContext.complexFormat() ==
      Preferences::ComplexFormat::Real) {
    return false;
  }
  ProjectionContext projCtx = {
      .m_complexFormat = approximationContext.complexFormat(),
      .m_angleUnit = approximationContext.angleUnit(),
      .m_context = approximationContext.context()};
  Tree* e = tree()->cloneTree();
  Simplification::ToSystem(e, &projCtx);
  if (!projCtx.m_dimension.isScalar() ||
      Internal::Dimension::ListLength(e) !=
          Internal::Dimension::k_nonListListLength) {
    e->removeTree();
    return false;
  }
  Approximation::PrepareExpressionForApproximation(e, projCtx.m_complexFormat);
  std::complex<T> z = Approximation::RootPreparedToComplex<T>(e);
  e->removeTree();
  T b = z.imag();
  if (b == static_cast<T>(0.) || std::isinf(b) || std::isnan(b)) {
    return false;
  }
  T a = z.real();
  if (std::isinf(a) || std::isnan(a)) {
    return false;
  }
  if (returnRealPart) {
    *returnRealPart = a;
  }
  if (returnImagPart) {
    *returnImagPart = b;
  }
  return true;
}

bool UserExpression::isScalarComplex(
    Preferences::CalculationPreferences calculationPreferences) const {
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  Preferences::AngleUnit angleUnit = calculationPreferences.angleUnit;
  ApproximationContext approximationContext(nullptr, complexFormat, angleUnit);
  approximationContext.updateComplexFormat(*this);
  if (hasDefinedComplexApproximation<double>(approximationContext)) {
    assert(!hasUnit());
    return true;
  }
  return false;
}

bool SystemFunction::isDiscontinuousBetweenFloatValues(float x1,
                                                       float x2) const {
  return Continuity::IsDiscontinuousBetweenFloatValues(tree(), x1, x2);
}

template <typename T>
PointOrScalar<T> SystemFunction::approximateToPointOrScalarWithValue(
    T x) const {
  return Internal::Approximation::RootPreparedToPointOrScalar<T>(tree(), x);
}

template <typename T>
T SystemExpression::approximateToScalarJunior() const {
  Tree* clonedTree = tree()->cloneTree();
  Approximation::PrepareExpressionForApproximation(clonedTree,
                                                   ComplexFormat::Real);
  T result = Approximation::RootPreparedToReal<T>(clonedTree, NAN);
  clonedTree->removeTree();
  return result;
}

template <typename T>
T SystemFunction::approximateIntegralToScalar(
    const SystemExpression& lowerBound,
    const SystemExpression& upperBound) const {
  Tree* integralTree = PatternMatching::Create(
      KIntegral("x"_e, KA, KB, KC),
      {.KA = lowerBound.tree(), .KB = upperBound.tree(), .KC = tree()});
  Approximation::PrepareExpressionForApproximation(integralTree,
                                                   ComplexFormat::Real);
  T result = Approximation::RootPreparedToReal<T>(integralTree, NAN);
  integralTree->removeTree();
  return result;
}

template <typename T>
SystemExpression SystemExpression::approximateListAndSort() const {
  assert(dimension().isList());
  Tree* clone = SharedTreeStack->pushListSort();
  tree()->cloneTree();
  clone->moveTreeOverTree(Approximation::RootTreeToTree<T>(clone));
  return SystemExpression::Builder(clone);
}

SystemExpression SystemExpression::removeUndefListElements() const {
  Tree* clone = tree()->cloneTree();
  assert(clone->isList());
  int n = clone->numberOfChildren();
  int remainingChildren = n;
  Tree* child = clone->nextNode();
  for (int i = 0; i < n; i++) {
    if (child->isUndefined() ||
        (child->isPoint() && child->hasChildSatisfying([](const Tree* e) {
          return e->isUndefined();
        }))) {
      child->removeTree();
      remainingChildren--;
      NAry::SetNumberOfChildren(clone, remainingChildren);
    } else {
      child = child->nextTree();
    }
  }
  return SystemExpression::Builder(clone);
}

UserExpression UserExpression::cloneAndSimplify(
    ReductionContext reductionContext, bool* reductionFailure) const {
  bool reduceFailure = false;
  UserExpression e =
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
  assert(!reduceFailure && (!tree()->isStore()));
#endif
  /* TODO_PCJ: e takes space in the pool only to be deleted right after
   * beautification. This could be optimized. */
  return e.cloneAndBeautify(reductionContext);
}

UserExpression ProjectedExpression::cloneAndBeautify(
    const ReductionContext& reductionContext) const {
  ProjectionContext context = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  Tree* e = tree()->cloneTree();
  Simplification::BeautifyReduced(e, &context);
  return Builder(e);
}

bool NewExpression::derivate(const ReductionContext& reductionContext,
                             Poincare::Symbol symbol, OExpression symbolValue) {
  // TODO_PCJ: Remove
  assert(false);
  return false;
}

int SystemExpression::polynomialDegree(const char* symbolName) const {
  return Degree::Get(tree(), symbolName);
}

int SystemExpression::getPolynomialReducedCoefficients(
    const char* symbolName, SystemExpression coefficients[], Context* context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation,
    bool keepDependencies) const {
  Tree* coefList = PolynomialParser::GetReducedCoefficients(tree(), symbolName,
                                                            keepDependencies);
  if (!coefList) {
    return -1;
  }
  int degree = coefList->numberOfChildren() - 1;
  assert(degree >= 0);
  Tree* child = coefList->nextNode();
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = Builder(child);
  }
  coefList->removeNode();
  return degree;
}

Poincare::Layout UserExpression::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context, OMG::Base base) const {
  if (isUninitialized()) {
    return Poincare::Layout();
  }
  return node()->createLayout(floatDisplayMode, numberOfSignificantDigits,
                              context, base);
}

char* UserExpression::toLatex(char* buffer, int bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits,
                              Context* context) const {
  return LatexParser::LayoutToLatex(
      Rack::From(
          createLayout(floatDisplayMode, numberOfSignificantDigits, context)
              .tree()),
      buffer, buffer + bufferSize - 1);
}

NewExpression NewExpression::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const NewExpression& expression,
    bool onlySecondTerm) {
  /* TODO_PCJ: Handle functions and sequences as well. See
   * replaceSymbolWithExpression implementations. */
  if (isUninitialized()) {
    return *this;
  }
  assert(symbol.tree()->isUserNamed());
  Tree* result = tree()->cloneTree();
  assert(!onlySecondTerm || result->numberOfChildren() >= 2);
  if (Variables::ReplaceSymbolWithTree(
          onlySecondTerm ? result->child(1) : result, symbol.tree(),
          expression.tree())) {
    NewExpression res = Builder(result);
    replaceWithInPlace(res);
    return res;
  }
  result->removeTree();
  return *this;
}

bool UserExpression::replaceSymbols(Poincare::Context* context,
                                    SymbolicComputation symbolic) {
  /* Caution: must be called on an unprojected expression!
   * Indeed, the projection of the replacements has to be done at the same time
   * as the rest of the expression (otherwise inconsistencies could appear like
   * with random for example). */
  Tree* clone = tree()->cloneTree();
  bool didReplace = Projection::DeepReplaceUserNamed(clone, context, symbolic);
  *this = NewExpression::Builder(clone);
  return didReplace;
}

static bool IsIgnoredSymbol(const NewExpression* e,
                            JuniorExpression::IgnoredSymbols* ignoredSymbols) {
  if (!e->tree()->isUserSymbol()) {
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

bool NewExpression::recursivelyMatches(ExpressionTrinaryTest test,
                                       Context* context,
                                       SymbolicComputation replaceSymbols,
                                       void* auxiliary,
                                       IgnoredSymbols* ignoredSymbols) const {
  if (!context) {
    replaceSymbols = SymbolicComputation::DoNotReplaceAnySymbol;
  }
  if (IsIgnoredSymbol(this, ignoredSymbols)) {
    return false;
  }
  OMG::Troolean testResult = test(*this, context, auxiliary);
  if (testResult == OMG::Troolean::True) {
    return true;
  } else if (testResult == OMG::Troolean::False) {
    return false;
  }
  assert(testResult == OMG::Troolean::Unknown && !isUninitialized());

  // Handle dependencies, store, symbols and functions
  if (tree()->isDep() || tree()->isStore()) {
    return cloneChildAtIndex(0).recursivelyMatches(
        test, context, replaceSymbols, auxiliary, ignoredSymbols);
  }
  if (tree()->isUserSymbol() || tree()->isUserFunction()) {
    assert(replaceSymbols ==
               SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
           replaceSymbols ==
               SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
           // We need only those cases for now
           || replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol);
    if (replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol ||
        (replaceSymbols ==
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions &&
         tree()->isUserSymbol())) {
      return false;
    }
    assert(replaceSymbols ==
               SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition ||
           tree()->isUserFunction());
    return SymbolAbstract::matches(convert<const SymbolAbstract>(), test,
                                   context, auxiliary, ignoredSymbols);
  }

  /* TODO_PCJ: This is highly ineffective : each child of the tree is cloned on
   * the pool to be recursivelyMatched. We do so so that ExpressionTrinaryTest
   * can use Expression API. */
  const int childrenCount = tree()->numberOfChildren();

  bool isParametered = tree()->isParametric();
  // Run loop backwards to find lists and matrices quicker in NAry expressions
  for (int i = childrenCount - 1; i >= 0; i--) {
    if (isParametered && i == Parametric::k_variableIndex) {
      continue;
    }
    NewExpression childToAnalyze = cloneChildAtIndex(i);
    bool matches;
    if (isParametered && i == Parametric::FunctionIndex(tree())) {
      NewExpression symbolExpr = cloneChildAtIndex(Parametric::k_variableIndex);
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

bool NewExpression::recursivelyMatches(
    ExpressionTest test, Context* context,
    SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const NewExpression e, Context* context,
                                     void* auxiliary) {
    ExpressionTest* trueTest = static_cast<ExpressionTest*>(auxiliary);
    return (*trueTest)(e, context) ? OMG::Troolean::True
                                   : OMG::Troolean::Unknown;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool NewExpression::recursivelyMatches(
    SimpleExpressionTest test, Context* context,
    SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const NewExpression e, Context* context,
                                     void* auxiliary) {
    SimpleExpressionTest* trueTest =
        static_cast<SimpleExpressionTest*>(auxiliary);
    return (*trueTest)(e) ? OMG::Troolean::True : OMG::Troolean::Unknown;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool NewExpression::recursivelyMatches(ExpressionTestAuxiliary test,
                                       Context* context,
                                       SymbolicComputation replaceSymbols,
                                       void* auxiliary) const {
  struct Pack {
    ExpressionTestAuxiliary* test;
    void* auxiliary;
  };
  ExpressionTrinaryTest ternary = [](const NewExpression e, Context* context,
                                     void* pack) {
    ExpressionTestAuxiliary* trueTest =
        static_cast<ExpressionTestAuxiliary*>(static_cast<Pack*>(pack)->test);
    return (*trueTest)(e, context, static_cast<Pack*>(pack)->auxiliary)
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  };
  Pack pack{&test, auxiliary};
  return recursivelyMatches(ternary, context, replaceSymbols, &pack);
}

Poincare::Dimension NewExpression::dimension(Context* context) const {
  return Poincare::Dimension(*this, context);
}

Sign SystemExpression::sign() const { return GetSign(tree()); }

bool NewExpression::hasComplexI(Context* context,
                                SymbolicComputation replaceSymbols) const {
  return !isUninitialized() && recursivelyMatches(
                                   [](const NewExpression e, Context* context) {
                                     return e.tree()->isComplexI();
                                   },
                                   context, replaceSymbols);
}

bool NewExpression::hasUnit(bool ignoreAngleUnits, bool* hasAngleUnits,
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
      [](const NewExpression e, Context* context, void* arg) {
        Pack* pack = static_cast<Pack*>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool* hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.tree()->isUnit() &&
                (!pack->ignoreAngleUnits || !isAngleUnit)) ||
               e.tree()->isPhysicalConstant();
      },
      ctx,
      replaceSymbols
          ? SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
          : SymbolicComputation::DoNotReplaceAnySymbol,
      &pack);
}
bool NewExpression::isUndefined() const {
  // TODO_PCJ: this is terribly confusing. We should either:
  // - rename NewExpression::isUndefined() into something more specific
  // - create a Tree range for non-nonreal undefined
  return tree()->isUndefined() && !tree()->isNonReal();
}

bool NewExpression::isMatrix(Context* context) const {
  if (context) {
    // TODO_PCJ: implement isMatrix check with a context
    assert(false);
    return false;
  }
  return tree()->isMatrix();
}

bool NewExpression::isNAry() const { return tree()->isNAry(); }

bool NewExpression::isApproximate() const {
  return tree()->isDecimal() || tree()->isFloat() || tree()->isDoubleFloat();
}

bool SystemExpression::isPlusOrMinusInfinity() const {
  return Infinity::IsPlusOrMinusInfinity(tree());
}

bool NewExpression::isPercent() const {
  return tree()->isPercentSimple() || tree()->isPercentAddition();
}

bool NewExpression::isSequence() const { return tree()->isSequence(); }

bool NewExpression::isIntegral() const { return tree()->isIntegral(); }

bool NewExpression::isDiff() const { return tree()->isDiff(); }

bool NewExpression::isBoolean() const { return tree()->isBoolean(); }

bool NewExpression::isList() const { return tree()->isList(); }

bool NewExpression::isUserSymbol() const { return tree()->isUserSymbol(); }

bool NewExpression::isUserFunction() const { return tree()->isUserFunction(); }

bool NewExpression::isStore() const { return tree()->isStore(); }

bool NewExpression::isFactor() const { return tree()->isFactor(); }

bool NewExpression::isPoint() const { return tree()->isPoint(); }

bool NewExpression::isNonReal() const { return tree()->isNonReal(); }

bool NewExpression::isOpposite() const { return tree()->isOpposite(); }

bool NewExpression::isDiv() const { return tree()->isDiv(); }

bool NewExpression::isBasedInteger() const {
  return tree()->isRational() && tree()->isInteger();
}

bool NewExpression::isDep() const { return tree()->isDep(); }

bool NewExpression::isComparison() const { return tree()->isComparison(); }

bool NewExpression::isRational() const { return tree()->isRational(); }

bool NewExpression::isPureAngleUnit() const {
  return !isUninitialized() && tree()->isUnit() &&
         Internal::Dimension::Get(tree()).isSimpleAngleUnit();
}

bool NewExpression::isInRadians(Context* context) const {
  return Internal::Dimension::Get(tree(), context).isSimpleRadianAngleUnit();
}

bool NewExpression::involvesDiscontinuousFunction(Context* context) const {
  return recursivelyMatches(IsDiscontinuous, context);
}

bool NewExpression::IsDiscontinuous(const NewExpression e, Context* context) {
  return Continuity::InvolvesDiscontinuousFunction(e.tree());
}

bool NewExpression::isConstantNumber() const {
  return !isUninitialized() && (tree()->isNumber() || tree()->isInf() ||
                                tree()->isUndefined() || tree()->isDecimal());
};

bool NewExpression::allChildrenAreUndefined() const {
  return !tree()->hasChildSatisfying(
      [](const Tree* e) { return !e->isUndefined(); });
}

/* Matrix */

Poincare::Matrix Poincare::Matrix::Builder() {
  NewExpression expr =
      NewExpression::Builder(SharedTreeStack->pushMatrix(0, 0));
  return static_cast<Poincare::Matrix&>(expr);
}

void Poincare::Matrix::setDimensions(int rows, int columns) {
  assert(rows * columns == tree()->numberOfChildren());
  Tree* clone = tree()->cloneTree();
  Internal::Matrix::SetNumberOfColumns(clone, columns);
  Internal::Matrix::SetNumberOfRows(clone, rows);
  NewExpression temp = NewExpression::Builder(clone);
  *this = static_cast<Poincare::Matrix&>(temp);
}

bool Poincare::Matrix::isVector() const {
  const Tree* t = tree();
  return Internal::Matrix::NumberOfRows(t) == 1 ||
         Internal::Matrix::NumberOfColumns(t) == 1;
}

int Poincare::Matrix::numberOfRows() const {
  return Internal::Matrix::NumberOfRows(tree());
}

int Poincare::Matrix::numberOfColumns() const {
  return Internal::Matrix::NumberOfColumns(tree());
}

// TODO_PCJ: Rework this and its usage
void Poincare::Matrix::addChildAtIndexInPlace(NewExpression t, int index,
                                              int currentNumberOfChildren) {
  Tree* clone = tree()->cloneTree();
  TreeRef newChild = t.tree()->cloneTree();
  if (index >= clone->numberOfChildren()) {
    int rows = Internal::Matrix::NumberOfRows(clone);
    int columns = Internal::Matrix::NumberOfColumns(clone);
    for (int i = 1; i < columns; i++) {
      KUndef->cloneTree();
    }
    Internal::Matrix::SetNumberOfRows(clone, rows + 1);
  } else {
    Tree* previousChild = clone->child(index);
    previousChild->removeTree();
    newChild->moveTreeAtNode(previousChild);
  }
  NewExpression temp = NewExpression::Builder(clone);
  *this = static_cast<Poincare::Matrix&>(temp);
}

NewExpression Poincare::Matrix::matrixChild(int i, int j) {
  return NewExpression::Builder(Internal::Matrix::Child(tree(), i, j));
}

int Poincare::Matrix::rank(Context* context, bool forceCanonization) {
  if (!forceCanonization) {
    return Internal::Matrix::Rank(tree());
  }
  Tree* clone = tree()->cloneTree();
  int result = Internal::Matrix::CanonizeAndRank(clone);
  NewExpression temp = NewExpression::Builder(clone);
  *this = static_cast<Poincare::Matrix&>(temp);
  return result;
}

/* Point */

Point Point::Builder(const Tree* x, const Tree* y) {
  Tree* tree = KPoint->cloneNode();
  x->cloneTree();
  y->cloneTree();
  NewExpression temp = NewExpression::Builder(tree);
  return static_cast<Point&>(temp);
}

template <typename T>
Coordinate2D<T> Point::approximate2D(
    const ApproximationContext& approximationContext) {
  // TODO_PCJ: Add context for angle unit and complex format.
  return Coordinate2D<T>(Approximation::RootTreeToReal<T>(tree()->child(0)),
                         Approximation::RootTreeToReal<T>(tree()->child(1)));
}

Poincare::Layout Point::create2DLayout(
    Preferences::PrintFloatMode floatDisplayMode, int significantDigits,
    Context* context) const {
  Poincare::Layout child0 = cloneChildAtIndex(0).createLayout(
      floatDisplayMode, significantDigits, context);
  Poincare::Layout child1 = cloneChildAtIndex(1).createLayout(
      floatDisplayMode, significantDigits, context);
  return Poincare::Layout::Create(KPoint2DL(KA, KB),
                                  {.KA = child0, .KB = child1});
}

template Coordinate2D<float> Point::approximate2D<float>(
    const ApproximationContext& approximationContext);
template Coordinate2D<double> Point::approximate2D<double>(
    const ApproximationContext& approximationContext);

/* List */

List List::Builder() {
  NewExpression expr = NewExpression::Builder(SharedTreeStack->pushList(0));
  return static_cast<List&>(expr);
}

void List::removeChildAtIndexInPlace(int i) {
  Tree* clone = tree()->cloneTree();
  NAry::RemoveChildAtIndex(clone, i);
  NewExpression temp = NewExpression::Builder(clone);
  *this = static_cast<List&>(temp);
}

// TODO_PCJ: Rework this and its usage
void List::addChildAtIndexInPlace(NewExpression t, int index,
                                  int currentNumberOfChildren) {
  Tree* clone = tree()->cloneTree();
  TreeRef newChild = t.tree()->cloneTree();
  NAry::SetNumberOfChildren(clone, clone->numberOfChildren() + 1);
  NewExpression temp = NewExpression::Builder(clone);
  *this = static_cast<List&>(temp);
}

int List::numberOfChildren() const { return tree()->numberOfChildren(); }

/* Boolean */

bool Boolean::value() const {
  assert(tree()->isTrue() || tree()->isFalse());
  return tree()->isTrue();
}

/* Unit */

NewExpression Unit::Builder(Preferences::AngleUnit angleUnit) {
  return NewExpression::Builder(Units::Unit::Push(angleUnit));
}

bool Unit::IsPureAngleUnit(NewExpression expression, bool isRadian) {
  return Units::IsPureAngleUnit(expression.tree()) &&
         (!isRadian || Units::Unit::GetRepresentative(expression.tree()) ==
                           &Units::Angle::representatives.radian);
}

bool Unit::HasAngleDimension(NewExpression expression) {
  assert(Internal::Dimension::DeepCheck(expression.tree()));
  return Internal::Dimension::Get(expression.tree()).isSimpleAngleUnit();
}

template SystemExpression JuniorExpressionNode::approximateToTree<float>(
    const ApproximationContext&) const;
template SystemExpression JuniorExpressionNode::approximateToTree<double>(
    const ApproximationContext&) const;

template Evaluation<float> EvaluationFromTree<float>(
    const Tree*, const ApproximationContext&);
template Evaluation<double> EvaluationFromTree<double>(
    const Tree*, const ApproximationContext&);

template Evaluation<float> EvaluationFromSimpleTree<float>(const Tree*);
template Evaluation<double> EvaluationFromSimpleTree<double>(const Tree*);

template SystemExpression SystemExpression::Builder<float>(float);
template SystemExpression SystemExpression::Builder<double>(double);
template SystemExpression SystemExpression::Builder<float>(Coordinate2D<float>);
template SystemExpression SystemExpression::Builder<double>(
    Coordinate2D<double>);
template SystemExpression SystemExpression::Builder<float>(
    PointOrScalar<float>);
template SystemExpression SystemExpression::Builder<double>(
    PointOrScalar<double>);

template PointOrScalar<float>
SystemFunction::approximateToPointOrScalarWithValue<float>(float) const;
template PointOrScalar<double>
SystemFunction::approximateToPointOrScalarWithValue<double>(double) const;

template SystemExpression SystemExpression::approximateListAndSort<float>()
    const;
template SystemExpression SystemExpression::approximateListAndSort<double>()
    const;

template float SystemFunction::approximateToScalarWithValue<float>(float,
                                                                   int) const;
template double SystemFunction::approximateToScalarWithValue<double>(double,
                                                                     int) const;

template float SystemExpression::approximateToScalarJunior<float>() const;
template double SystemExpression::approximateToScalarJunior<double>() const;

template float SystemFunction::approximateIntegralToScalar<float>(
    const SystemExpression& upperBound,
    const SystemExpression& lowerBound) const;
template double SystemFunction::approximateIntegralToScalar<double>(
    const SystemExpression& upperBound,
    const SystemExpression& lowerBound) const;

template float UserExpression::ParseAndSimplifyAndApproximateToScalar<float>(
    const char*, Context*, SymbolicComputation);
template double UserExpression::ParseAndSimplifyAndApproximateToScalar<double>(
    const char*, Context*, SymbolicComputation);

template bool UserExpression::hasDefinedComplexApproximation<float>(
    const ApproximationContext&, float*, float*) const;
template bool UserExpression::hasDefinedComplexApproximation<double>(
    const ApproximationContext&, double*, double*) const;

template float UserExpression::approximateUserExpressionToScalar<float>(
    Preferences::AngleUnit, Preferences::ComplexFormat) const;
template double UserExpression::approximateUserExpressionToScalar<double>(
    Preferences::AngleUnit, Preferences::ComplexFormat) const;

}  // namespace Poincare
