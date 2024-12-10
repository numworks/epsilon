#include <poincare/cas.h>
#include <poincare/helpers/symbol.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/continuity.h>
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
      m_isValid(false),
      m_isEmptyList(false) {
  // TODO_PCJ: Remove checks in ProjectedExpression implementation of this
  if (!Internal::Dimension::DeepCheck(e.tree(), context)) {
    return;
  }
  Internal::Dimension dimension = Internal::Dimension::Get(e.tree(), context);
  m_type = dimension.type;
  m_isList = Internal::Dimension::IsList(e.tree(), context);
  m_isEmptyList = (m_isList && Internal::Dimension::ListLength(e.tree()) == 0);
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

bool Poincare::Dimension::isEmptyList() { return m_isEmptyList; }

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

template <typename T>
SystemExpression JuniorExpressionNode::approximateToTree(
    const ApproximationContext& approximationContext) const {
  return SystemExpression::Builder(Approximation::ToTree<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true},
      Approximation::Context(approximationContext.angleUnit(),
                             approximationContext.complexFormat(),
                             approximationContext.context())));
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

const Tree* JuniorExpressionNode::tree() const {
  return Tree::FromBlocks(m_blocks);
}

/* JuniorExpression */

bool JuniorExpression::isIdenticalTo(const JuniorExpression e) const {
  return tree()->treeIsIdenticalTo(e.tree());
}

NewExpression NewExpression::ExpressionFromAddress(const void* address,
                                                   size_t size) {
  if (address == nullptr || size == 0) {
    return NewExpression();
  }
  // Build the OExpression in the Tree Pool
  return NewExpression(static_cast<JuniorExpressionNode*>(
      Pool::sharedPool->copyTreeFromAddress(address, size)));
}

const Tree* NewExpression::TreeFromAddress(const void* address) {
  if (address == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<const JuniorExpressionNode*>(address)->tree();
}

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

NewExpression NewExpression::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

int NewExpression::numberOfDescendants(bool includeSelf) const {
  assert(tree());
  return tree()->numberOfDescendants(includeSelf);
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
      context, SymbolicComputation::ReplaceDefinedSymbols, &types);
}

void UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression,
    Internal::ProjectionContext* context, bool* reductionFailure) const {
  // Step 1: simplify
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  *simplifiedExpression = cloneAndSimplify(context, reductionFailure);
  assert(!simplifiedExpression->isUninitialized());
  // Step 2: approximate
  assert(!approximatedExpression || approximatedExpression->isUninitialized());
  if (approximatedExpression) {
    const Tree* e = simplifiedExpression->tree();
    Approximation::Context approxCtx(context->m_angleUnit,
                                     context->m_complexFormat);
    if (CAS::Enabled()) {
      Tree* a = e->cloneTree();
      /* We are using ApproximateAndReplaceEveryScalar to approximate
       * expressions with symbols such as π*x → 3.14*x. */
      Approximation::ApproximateAndReplaceEveryScalar<double>(a, approxCtx);
      *approximatedExpression = UserExpression::Builder(a);
    } else {
      Tree* a = Approximation::ToTree<double>(
          e,
          Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                    .projectLocalVariables = true},
          approxCtx);
      context->m_dimension = Internal::Dimension::Get(a);
      Beautification::DeepBeautify(a, *context);
      *approximatedExpression = UserExpression::Builder(a);
    }
  }
  return;
}

UserExpression UserExpression::cloneAndSimplify(
    Internal::ProjectionContext* context, bool* reductionFailure) const {
  return cloneAndReduceAndBeautify(context, true, reductionFailure);
}

SystemExpression UserExpression::cloneAndReduce(
    ReductionContext reductionContext, bool* reductionFailure) const {
  ProjectionContext context = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_expansionStrategy =
          (reductionContext.target() == ReductionTarget::SystemForAnalysis)
              ? Poincare::Internal::ExpansionStrategy::ExpandAlgebraic
              : Poincare::Internal::ExpansionStrategy::None,
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  return cloneAndReduceAndBeautify(&context, false, reductionFailure);
}

/* TODO_PCJ: refactor into cloneAndReduce and cloneAndReduceAndBeautify to
 * return correct expression type (UserExpression if beautified,
 * SystemExpression otherwise) */
SystemExpression UserExpression::cloneAndReduceAndBeautify(
    Internal::ProjectionContext* context, bool beautify,
    bool* reductionFailure) const {
  assert(!isUninitialized());
  Tree* e = tree()->cloneTree();
  // TODO_PCJ: Decide if a projection is needed or not
  bool reductionSuccess = Simplification::Simplify(e, *context, beautify);
  if (reductionFailure) {
    *reductionFailure = !reductionSuccess;
  }
  return Builder(e);
}

UserExpression ProjectedExpression::cloneAndBeautify(
    const ReductionContext& reductionContext) const {
  ProjectionContext context = {
      .m_complexFormat = reductionContext.complexFormat(),
      .m_angleUnit = reductionContext.angleUnit(),
      .m_expansionStrategy =
          (reductionContext.target() == ReductionTarget::SystemForAnalysis)
              ? Poincare::Internal::ExpansionStrategy::ExpandAlgebraic
              : Poincare::Internal::ExpansionStrategy::None,
      .m_unitFormat = reductionContext.unitFormat(),
      .m_symbolic = reductionContext.symbolicComputation(),
      .m_context = reductionContext.context()};
  Tree* e = tree()->cloneTree();
  context.m_dimension = Internal::Dimension::Get(e);
  Simplification::BeautifyReduced(e, &context);
  return Builder(e);
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
  // Check dimension again as the diff may have changed it.
  if (!Internal::Dimension::DeepCheck(result)) {
    result->cloneTreeOverTree(KUndefUnhandledDimension);
  } else {
    Simplification::ReduceSystem(result, false);
  }
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
T UserExpression::approximateToScalar(AngleUnit angleUnit,
                                      ComplexFormat complexFormat,
                                      Context* context) const {
  return Approximation::To<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, context));
}

template <typename T>
T SystemFunctionScalar::approximateToScalarWithValue(T x,
                                                     int listElement) const {
  return Approximation::To<T>(
      tree(), x, Approximation::Parameters{.isRootAndCanHaveRandom = true},
      Approximation::Context(AngleUnit::None, ComplexFormat::None, nullptr,
                             listElement));
}

template <typename T>
T UserExpression::ParseAndSimplifyAndApproximateToScalar(
    const char* text, Context* context,
    SymbolicComputation symbolicComputation) {
  UserExpression exp = Parse(text, context, false);
  if (exp.isUninitialized()) {
    return NAN;
  }
  ProjectionContext ctx = {
      .m_complexFormat = Preferences::SharedPreferences()->complexFormat(),
      .m_angleUnit = Preferences::SharedPreferences()->angleUnit(),
      .m_symbolic = symbolicComputation,
      .m_context = context};
  exp = exp.cloneAndSimplify(&ctx);
  assert(!exp.isUninitialized());
  return exp.approximateToScalar<T>(ctx.m_angleUnit, ctx.m_complexFormat);
}

template <typename T>
bool UserExpression::hasDefinedComplexApproximation(
    const ApproximationContext& approximationContext, T* returnRealPart,
    T* returnImagPart) const {
  if (approximationContext.complexFormat() ==
          Preferences::ComplexFormat::Real ||
      !Internal::Dimension::IsNonListScalar(tree())) {
    return false;
  }
  // TODO_PCJ: Remove ApproximationContext
  std::complex<T> z = Approximation::ToComplex<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(approximationContext.angleUnit(),
                             approximationContext.complexFormat(),
                             approximationContext.context()));
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
    Preferences::CalculationPreferences calculationPreferences,
    Context* context) const {
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  Preferences::AngleUnit angleUnit = calculationPreferences.angleUnit;
  ApproximationContext approximationContext(context, complexFormat, angleUnit);
  approximationContext.updateComplexFormat(*this);
  if (hasDefinedComplexApproximation<double>(approximationContext)) {
    assert(!hasUnit());
    return true;
  }
  return false;
}

bool SystemFunction::isDiscontinuousBetweenFloatValues(float x1,
                                                       float x2) const {
  return Continuity::IsDiscontinuousBetweenValues<float>(tree(), x1, x2);
}

template <typename T>
PointOrScalar<T> SystemFunction::approximateToPointOrScalarWithValue(
    T x) const {
  return Internal::Approximation::ToPointOrScalar<T>(
      tree(), x, Approximation::Parameters{.isRootAndCanHaveRandom = true});
}

template <typename T>
T SystemExpression::approximateSystemToScalar() const {
  return Approximation::To<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = true});
}

template <typename T>
Coordinate2D<T> SystemExpression::approximateToPoint() const {
  return Approximation::ToPoint<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = true});
}

template <typename T>
T SystemFunction::approximateIntegralToScalar(
    const SystemExpression& lowerBound,
    const SystemExpression& upperBound) const {
  Tree* integralTree = PatternMatching::Create(
      KIntegral("x"_e, KA, KB, KC),
      {.KA = lowerBound.tree(), .KB = upperBound.tree(), .KC = tree()});
  T result = Approximation::To<T>(
      integralTree, Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                              .prepare = true});
  integralTree->removeTree();
  return result;
}

template <typename T>
SystemExpression SystemExpression::approximateListAndSort() const {
  assert(dimension().isList());
  Tree* clone = SharedTreeStack->pushListSort();
  tree()->cloneTree();
  clone->moveTreeOverTree(Approximation::ToTree<T>(
      clone, Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                       .prepare = true}));
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
                              int numberOfSignificantDigits, Context* context,
                              bool withThousandsSeparator) const {
  return LatexParser::LayoutToLatex(
      Rack::From(
          createLayout(floatDisplayMode, numberOfSignificantDigits, context)
              .tree()),
      buffer, buffer + bufferSize - 1, withThousandsSeparator);
}

size_t UserExpression::serialize(char* buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return isUninitialized()
             ? 0
             : node()->serialize(buffer, bufferSize, floatDisplayMode,
                                 numberOfSignificantDigits);
}

Ion::Storage::Record::ErrorStatus UserExpression::storeWithNameAndExtension(
    const char* baseName, const char* extension) const {
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      baseName, extension, addressInPool(), size(), true);
}

bool NewExpression::replaceSymbolWithExpression(
    const JuniorSymbolAbstract& symbol, const NewExpression& expression,
    bool onlySecondTerm) {
  /* TODO_PCJ: Handle functions and sequences as well. See
   * replaceSymbolWithExpression implementations. */
  if (isUninitialized()) {
    return false;
  }
  assert(symbol.tree()->isUserNamed());
  Tree* result = tree()->cloneTree();
  assert(!onlySecondTerm || result->numberOfChildren() >= 2);
  if (Variables::ReplaceSymbolWithTree(
          onlySecondTerm ? result->child(1) : result, symbol.tree(),
          expression.tree())) {
    NewExpression res = Builder(result);
    replaceWithInPlace(res);
    *this = res;
    return true;
  }
  result->removeTree();
  return false;
}

bool NewExpression::replaceSymbolWithUnknown(const JuniorSymbolAbstract& symbol,
                                             bool onlySecondTerm) {
  return replaceSymbolWithExpression(symbol, JuniorSymbol::SystemSymbol(),
                                     onlySecondTerm);
}

bool NewExpression::replaceUnknownWithSymbol(CodePoint symbol) {
  return replaceSymbolWithExpression(JuniorSymbol::SystemSymbol(),
                                     JuniorSymbol::Builder(symbol));
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
    replaceSymbols = SymbolicComputation::KeepAllSymbols;
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
    assert(replaceSymbols == SymbolicComputation::ReplaceDefinedSymbols ||
           replaceSymbols == SymbolicComputation::ReplaceDefinedFunctions
           // We need only those cases for now
           || replaceSymbols == SymbolicComputation::KeepAllSymbols);
    if (replaceSymbols == SymbolicComputation::KeepAllSymbols ||
        (replaceSymbols == SymbolicComputation::ReplaceDefinedFunctions &&
         tree()->isUserSymbol())) {
      return false;
    }
    assert(replaceSymbols == SymbolicComputation::ReplaceDefinedSymbols ||
           tree()->isUserFunction());
    JuniorExpression e = clone();
    // Undefined symbols must be preserved.
    e.replaceSymbols(context, SymbolicComputation::ReplaceDefinedSymbols);
    return !e.isUninitialized() &&
           e.recursivelyMatches(test, context,
                                SymbolicComputation::KeepAllSymbols, auxiliary,
                                ignoredSymbols);
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

bool NewExpression::recursivelyMatches(
    NonStaticSimpleExpressionTest test, Context* context,
    SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const NewExpression e, Context* context,
                                     void* auxiliary) {
    NonStaticSimpleExpressionTest* trueTest =
        static_cast<NonStaticSimpleExpressionTest*>(auxiliary);
    return (e.**trueTest)() ? OMG::Troolean::True : OMG::Troolean::Unknown;
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
        return (e.tree()->isUnitOrPhysicalConstant() &&
                (!pack->ignoreAngleUnits || !isAngleUnit)) ||
               e.tree()->isPhysicalConstant();
      },
      ctx,
      replaceSymbols ? SymbolicComputation::ReplaceDefinedSymbols
                     : SymbolicComputation::KeepAllSymbols,
      &pack);
}
bool NewExpression::isUndefined() const {
  // TODO_PCJ: this is terribly confusing. We should either:
  // - rename NewExpression::isUndefined() into something more specific
  // - create a Tree range for non-nonreal undefined
  return tree()->isUndefined() && !tree()->isNonReal();
}
bool NewExpression::isUndefinedOrNonReal() const {
  return tree()->isUndefined();
}

bool NewExpression::isMatrix(Context* context) const {
  if (context) {
    // TODO_PCJ: implement isMatrix check with a context
    assert(false);
    return false;
  }
  return tree()->isMatrix();
}

bool NewExpression::isDiscontinuous() const {
  return Continuity::InvolvesDiscontinuousFunction(tree());
}

bool NewExpression::isNAry() const { return tree()->isNAry(); }

bool NewExpression::isApproximate() const {
  return tree()->isDecimal() || tree()->isFloat() || tree()->isDoubleFloat();
}

bool SystemExpression::isPlusOrMinusInfinity() const {
  return Internal::Infinity::IsPlusOrMinusInfinity(tree());
}

bool NewExpression::isPercent() const {
  return tree()->isPercentSimple() || tree()->isPercentAddition();
}

bool NewExpression::isSequence() const { return tree()->isSequence(); }

bool NewExpression::isParametric() const { return tree()->isParametric(); }

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

bool NewExpression::isEquality() const { return tree()->isEqual(); }

bool NewExpression::isRational() const { return tree()->isRational(); }

bool NewExpression::isPureAngleUnit() const {
  return !isUninitialized() && tree()->isUnit() &&
         Internal::Dimension::Get(tree()).isSimpleAngleUnit();
}

bool NewExpression::isInRadians(Context* context) const {
  return Internal::Dimension::Get(tree(), context).isSimpleRadianAngleUnit();
}

bool NewExpression::involvesDiscontinuousFunction(Context* context) const {
  return recursivelyMatches(&JuniorExpression::isDiscontinuous, context);
}

bool NewExpression::isConstantNumber() const {
  return !isUninitialized() && (tree()->isNumber() || tree()->isInf() ||
                                tree()->isUndefined() || tree()->isDecimal());
};

bool NewExpression::allChildrenAreUndefined() const {
  return !tree()->hasChildSatisfying(
      [](const Tree* e) { return !e->isUndefined(); });
}

bool NewExpression::hasRandomNumber() const {
  return !tree()->hasDescendantSatisfying(
      [](const Tree* e) { return !e->isRandom() || e->isRandInt(); });
}

bool NewExpression::hasRandomList() const {
  return !tree()->hasDescendantSatisfying(
      [](const Tree* e) { return !e->isRandIntNoRep(); });
}

ComparisonJunior::Operator NewExpression::comparisonOperator() const {
  assert(isComparison());
  return Internal::Binary::ComparisonOperatorForType(tree()->type());
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
/* Undefined */

Undefined Undefined::Builder() {
  NewExpression e = NewExpression::Builder(KUndef);
  return static_cast<Undefined&>(e);
}

/* NonReal */

NonReal NonReal::Builder() {
  NewExpression e = NewExpression::Builder(KNonReal);
  return static_cast<NonReal&>(e);
}

/* Infinity */

const char* Poincare::Infinity::k_infinityName =
    Internal::Infinity::k_infinityName;
const char* Poincare::Infinity::k_minusInfinityName =
    Internal::Infinity::k_minusInfinityName;

/* JuniorSymbolAbstract */

const char* JuniorSymbolAbstract::name() const {
  return SymbolHelper::GetName(*this);
}

/* JuniorSymbol */

JuniorSymbol JuniorSymbol::Builder(const char* name, int length) {
  if (length < 0) {
    length = strlen(name);
  }
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
    length = strlen(name);
  }
  NewExpression e = JuniorExpression::Builder(
      SharedTreeStack->pushUserSymbol(name, static_cast<size_t>(length + 1)));
  return static_cast<JuniorSymbol&>(e);
}

JuniorSymbol JuniorSymbol::Builder(CodePoint name) {
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointLength =
      UTF8Helper::WriteCodePoint(buffer, bufferSize - 1, name);
  assert(codePointLength < bufferSize);
  return Builder(buffer, codePointLength);
}

JuniorSymbol JuniorSymbol::Ans() {
  return Builder(SymbolHelper::AnsMainAlias());
}

/* JuniorFunction */

JuniorFunction JuniorFunction::Builder(const char* name,
                                       JuniorExpression child) {
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
  }
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserFunction(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  JuniorExpression expr = JuniorExpression::Builder(e);
  return static_cast<JuniorFunction&>(expr);
}

/* JuniorSequence */

JuniorSequence JuniorSequence::Builder(const char* name,
                                       JuniorExpression child) {
  // If needed, handle theta like functions and symbols
  assert(!BuiltinsAliases::k_thetaAliases.contains(name));
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserSequence(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  JuniorExpression expr = JuniorExpression::Builder(e);
  return static_cast<JuniorSequence&>(expr);
}

template SystemExpression JuniorExpressionNode::approximateToTree<float>(
    const ApproximationContext&) const;
template SystemExpression JuniorExpressionNode::approximateToTree<double>(
    const ApproximationContext&) const;

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

template float SystemFunctionScalar::approximateToScalarWithValue<float>(
    float, int) const;
template double SystemFunctionScalar::approximateToScalarWithValue<double>(
    double, int) const;

template float SystemExpression::approximateSystemToScalar<float>() const;
template double SystemExpression::approximateSystemToScalar<double>() const;

template Coordinate2D<float> SystemExpression::approximateToPoint<float>()
    const;
template Coordinate2D<double> SystemExpression::approximateToPoint<double>()
    const;

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

template float UserExpression::approximateToScalar<float>(
    Preferences::AngleUnit, Preferences::ComplexFormat, Context*) const;
template double UserExpression::approximateToScalar<double>(
    Preferences::AngleUnit, Preferences::ComplexFormat, Context*) const;

}  // namespace Poincare
