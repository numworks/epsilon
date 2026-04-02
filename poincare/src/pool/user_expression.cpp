#include <poincare/cas.h>
#include <poincare/code_points.h>
#include <poincare/helpers/layout.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/layout.h>
#include <poincare/simplification_parameters.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/layout_serializer.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/latex_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/system_expression.h>
#include <poincare/user_expression.h>

namespace Poincare {

using namespace Internal;

UserExpression UserExpression::ExpressionFromAddress(const void* address,
                                                     size_t size) {
  if (address == nullptr || size == 0) {
    return UserExpression();
  }
  // Build the Expression in the Pool
  Expression e = Expression(static_cast<ExpressionObject*>(
      Pool::sharedPool->copyObjectFromAddress(address, size)));
  return static_cast<UserExpression&>(e);
}

UserExpression UserExpression::Parse(const Tree* layout,
                                     const SymbolContext& symbolContext,
                                     ParserHelper::ParsingParameters params) {
  return UserExpression::Builder(Parser::Parse(layout, symbolContext, params));
}

UserExpression UserExpression::Parse(const char* string,
                                     const SymbolContext& symbolContext,
                                     ParserHelper::ParsingParameters params) {
  if (string[0] == 0) {
    return UserExpression();
  }
  Tree* layout = RackFromText(string);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result = Parse(layout, symbolContext, params);
  layout->removeTree();
  return result;
}

UserExpression UserExpression::ParseLatex(
    const char* latex, const SymbolContext& symbolContext,
    ParserHelper::ParsingParameters params) {
  Tree* layout = LatexParser::LatexToLayout(latex);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result = Parse(layout, symbolContext, params);
  layout->removeTree();
  return result;
}

UserExpression UserExpression::Create(const Tree* structure,
                                      ContextTrees ctxTrees) {
  /* Since we build a [NoScopeContext], it is expected that the trees of [ctx]
   * come from UserExpression */
  return UserExpression::Builder(PatternMatching::Create(
      structure, PatternMatching::Context::NoScopeContext(ctxTrees)));
}

UserExpression UserExpression::Builder(int32_t n) {
  return Builder(Integer::Push(n));
}

template <typename T>
UserExpression UserExpression::Builder(T x) {
  return Builder(SharedTreeStack->pushFloat(x));
}

template <typename T>
UserExpression UserExpression::Builder(PointOrRealScalar<T> pointOrRealScalar) {
  if (pointOrRealScalar.isRealScalar()) {
    return Builder<T>(pointOrRealScalar.toRealScalar());
  }
  return Builder<T>(pointOrRealScalar.toPoint());
}

template <typename T>
UserExpression UserExpression::Builder(Coordinate2D<T> point) {
  return UserExpression::Create(KPoint(KA, KB), {.KA = Builder<T>(point.x()),
                                                 .KB = Builder<T>(point.y())});
}

UserExpression UserExpression::BuildListOfExpressions(
    std::span<UserExpression> expressions) {
  Internal::Tree* list = Internal::SharedTreeStack->pushList(0);
  for (const UserExpression& expr : expressions) {
    expr.tree()->cloneTree();
  }
  Internal::NAry::SetNumberOfChildren(list, expressions.size());
  return Builder(list);
}

UserExpression UserExpression::Undefined() {
  return UserExpression::Builder(KUndef);
}

UserExpression UserExpression::Builder(AngleUnit angleUnit) {
  return UserExpression::Builder(Units::Unit::Push(angleUnit));
}

UserExpression UserExpression::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

bool UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression,
    const SimplificationParameters& simplificationParameters,
    const ApproximationPolicy& approximationPolicy) const {
  // Step 1: simplify
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  bool reductionFailure = false;
  *simplifiedExpression =
      cloneAndSimplify(simplificationParameters, &reductionFailure);
  assert(!simplifiedExpression->isUninitialized());
  // Step 2: approximate
  assert(approximatedExpression && approximatedExpression->isUninitialized());
  *approximatedExpression = simplifiedExpression->cloneAndApproximate(
      simplificationParameters.user, approximationPolicy);
  /* Second approximation attempt.
   * Condition: simplified expression is well defined, but approximation failed
   * (it returned undef). This can happen if the simplified expression is a
   * ratio of two very big numbers.
   * Second strategy: approximate from the input expression. */
  if (approximatedExpression->isUndefined() &&
      !simplifiedExpression->isUndefinedOrNonReal()) {
    *approximatedExpression = this->cloneAndApproximate(
        simplificationParameters.user, approximationPolicy);
  }
  return reductionFailure;
}

UserExpression UserExpression::cloneAndApproximate(
    const UserParameters& userParameters,
    const ApproximationPolicy& approximationPolicy,
    FloatPrecision floatPrecision) const {
  Tree* a = Approximation::To<Tree*>(
      tree(), floatPrecision,
      Approximation::Parameters{
          .policy = approximationPolicy,
          .expressionParameters =
              Approximation::ExpressionVariantParameters(userParameters)});
  /* There are several issues that currently prevent us from beautifying an
   * approximated CAS expression:
   * - the beautified exact expression and the beautified approximate expression
   * can be equal but in a different order, for example "12y^2 - 12y + 2/y^3 =
   * 12y^2 + 2/y^3 - 12y"
   * - the complex format detection during beautification can make an assertion
   * raise, for example "(x+1)*(pi+i)"
   * TODO: solve these issues */
  if (!CAS::Enabled()) {
    Beautification::DeepBeautify(
        a, BeautificationParameters{.user = userParameters},
        Internal::Dimension::Get(a));
  }
  return UserExpression::Builder(a);
}

UserExpression UserExpression::cloneAndSimplify(
    const SimplificationParameters& simplificationParameters,
    bool* reductionFailure) const {
  assert(reductionFailure);
  return privateCloneAndSimplify(simplificationParameters, reductionFailure);
}

UserExpression UserExpression::cloneAndTrySimplify(
    const SimplificationParameters& simplificationParameters) const {
  return privateCloneAndSimplify(simplificationParameters, nullptr);
}

SystemExpression UserExpression::cloneAndReduce(
    const SimplificationParameters& simplificationParameters) const {
  Tree* e = tree()->cloneTree();
  if (!Simplification::Simplify(e, simplificationParameters, false)) {
    e->cloneTreeOverTree(KFailedSimplification);
  }
  return SystemExpression::Builder(e);
}

UserExpression UserExpression::privateCloneAndSimplify(
    const SimplificationParameters& simplificationParameters,
    bool* reductionFailure) const {
  assert(!isUninitialized());
  Tree* e = tree()->cloneTree();
  bool reductionSuccess =
      Simplification::Simplify(e, simplificationParameters, true);
  if (reductionFailure) {
    *reductionFailure = !reductionSuccess;
  }
  return UserExpression::Builder(e);
}

template <typename T>
T UserExpression::privateApproximateToRealScalar(
    const UserParameters& userParameters,
    const ApproximationPolicy& approximationPolicy) const {
  static_assert(std::is_floating_point_v<T>);
  return Approximation::To<T>(
      tree(),
      Approximation::Parameters{.policy = approximationPolicy,
                                .expressionParameters = userParameters});
}

template <typename T>
T UserExpression::approximateToRealScalar(
    const UserParameters& userParameters,
    const ApproximationPolicy& approximationPolicy) const {
  static_assert(std::is_floating_point_v<T>);
  if (!Internal::Dimension::IsNonListScalar(tree(), userParameters.context)) {
    return NAN;
  }
  return privateApproximateToRealScalar<T>(userParameters, approximationPolicy);
}

template <typename T>
T UserExpression::approximateUnitToRealScalar(
    const UserParameters& userParameters,
    const ApproximationPolicy& approximationPolicy) const {
  static_assert(std::is_floating_point_v<T>);
  Poincare::Dimension dimension =
      Poincare::Dimension(*this, userParameters.context);
  if (!dimension.isUnit() ||
      Internal::Dimension::IsList(tree(), userParameters.context)) {
    return NAN;
  }
  return privateApproximateToRealScalar<T>(userParameters, approximationPolicy);
}

template <typename T>
T UserExpression::ParseAndSimplifyAndApproximateToRealScalar(
    const char* text,
    const SimplificationParameters& simplificationParameters) {
  UserExpression exp =
      UserExpression::Parse(text, simplificationParameters.user.context);
  if (exp.isUninitialized()) {
    return NAN;
  }
  bool reductionFailure;
  exp = exp.cloneAndSimplify(simplificationParameters, &reductionFailure);
  assert(!exp.isUninitialized());
  /* ApproximationPolicy is always optimized because dimension has been checked
   * during simplification */
  return exp.approximateToRealScalar<T>(simplificationParameters.user,
                                        ApproximationPolicy{.optimize = true});
}

constexpr char ordinateName[] = {CodePoints::k_ordinateSymbol, 0};

UserExpression UserExpression::equivalentCartesianEquation() const {
  const Tree* t = tree();
  // Looking for KSub("y"_e, [something without "y"_e])
  if (t->isSub() && Symbol::IsUserVariable(t->child(0), ordinateName) &&
      !(t->child(1)->hasDescendantSatisfying([](const Tree* e) {
        return Symbol::IsUserVariable(e, ordinateName);
      }))) {
    return UserExpression::Builder(t->child(1));
  }
  return UserExpression();
}

ComplexFormat UserExpression::preferredComplexFormat(
    ComplexFormat userComplexFormat, const SymbolContext& symbolContext,
    SymbolicComputation symbolicComputation) const {
  return Internal::Projection::PreferredComplexFormat(
      tree(), userComplexFormat, symbolicComputation, symbolContext);
}

template <typename T>
bool UserExpression::hasDefinedComplexApproximation(
    AngleUnit angleUnit, ComplexFormat complexFormat,
    const SymbolContext& symbolContext, T* returnRealPart,
    T* returnImagPart) const {
  if (complexFormat == ComplexFormat::Real ||
      !Internal::Dimension::IsNonListScalar(tree())) {
    return false;
  }
  std::complex<T> z = Approximation::To<std::complex<T>>(
      tree(), Approximation::Parameters{.expressionParameters = UserParameters{
                                            .complexFormat = complexFormat,
                                            .angleUnit = angleUnit,
                                            .context = symbolContext}});
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

bool UserExpression::isComplexScalar(
    Preferences::CalculationPreferences calculationPreferences,
    const SymbolContext& symbolContext) const {
  ComplexFormat complexFormat = preferredComplexFormat(
      calculationPreferences.complexFormat, symbolContext,
      SymbolicComputation::ReplaceDefinedSymbols);
  AngleUnit angleUnit = calculationPreferences.angleUnit;
  if (hasDefinedComplexApproximation<double>(angleUnit, complexFormat,
                                             symbolContext)) {
    assert(!hasUnit());
    return true;
  }
  return false;
}

Poincare::Layout UserExpression::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    const SymbolContext& symbolContext, OMG::Base base,
    bool useTenPowerInsteadOfE, bool linearMode) const {
  if (isUninitialized()) {
    return Poincare::Layout();
  }
  return Poincare::Layout::Builder(Layouter::LayoutExpression(
      tree(), {.symbolContext = symbolContext,
               .layouterMode =
                   linearMode ? LayouterMode::Linear : LayouterMode::Natural,
               .numberOfSignificantDigits = numberOfSignificantDigits,
               .floatMode = floatDisplayMode,
               .base = base,
               .useTenPowerInsteadOfE = useTenPowerInsteadOfE}));
}

char* UserExpression::toLatex(char* buffer, int bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits,
                              const SymbolContext& symbolContext,
                              bool withThousandsSeparator) const {
  return LatexParser::LayoutToLatex(
      Rack::From(createLayout(floatDisplayMode, numberOfSignificantDigits,
                              symbolContext)
                     .tree()),
      buffer, buffer + bufferSize - 1, withThousandsSeparator);
}

size_t UserExpression::serialize(std::span<char> buffer, bool compactMode,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  if (isUninitialized()) {
    return 0;
  }
  Tree* layout = Layouter::LayoutExpression(
      tree(), {.layouterMode = compactMode ? LayouterMode::LinearCompact
                                           : LayouterMode::Linear,
               .numberOfSignificantDigits = numberOfSignificantDigits});
  size_t length = LayoutSerializer::Serialize(layout, buffer);
  layout->removeTree();
  assert(length <= buffer.size() || length == LayoutHelpers::k_bufferOverflow);
  return length;
}

UserExpression UserExpression::cloneAndReplaceSymbolWithExpression(
    const UserExpression& symbol, const Expression& expression,
    bool onlySecondTerm) const {
  /* TODO_PCJ: Handle functions and sequences as well. See ReplaceSymbolWithTree
   * implementation. */
  if (isUninitialized()) {
    return UserExpression();
  }
  assert(symbol.tree()->isUserSymbol());
  Tree* result = tree()->cloneTree();
  assert(!onlySecondTerm || result->numberOfChildren() >= 2);
  Variables::ReplaceSymbolWithTree(onlySecondTerm ? result->child(1) : result,
                                   symbol.tree(), expression.tree());
  return UserExpression::Builder(result);
}

UserExpression UserExpression::cloneAndReplaceSymbolWithUnknown(
    const UserExpression& symbol, bool onlySecondTerm) const {
  return cloneAndReplaceSymbolWithExpression(
      symbol, SymbolHelper::SystemSymbol(), onlySecondTerm);
}

UserExpression UserExpression::cloneAndReplaceUnknownWithSymbol(
    CodePoint symbol) const {
  return cloneAndReplaceSymbolWithExpression(
      SymbolHelper::SystemSymbol(), SymbolHelper::BuildUserVariable(symbol));
}

UserExpression UserExpression::cloneAndReplaceSymbols(
    const Poincare::SymbolContext& symbolContext,
    SymbolicComputation symbolic) const {
  /* Caution: must be called on an unprojected expression!
   * Indeed, the projection of the replacements has to be done at the same time
   * as the rest of the expression (otherwise inconsistencies could appear like
   * with random for example). */
  Tree* clone = tree()->cloneTree();
  Projection::DeepReplaceUserSymbol(clone, symbolContext, symbolic);
  return UserExpression::Builder(clone);
}

struct IgnoredSymbols {
  const Internal::Tree* head;  // UserExpression tree
  void* tail;
};

bool IsIgnoredSymbol(const Tree* tree, IgnoredSymbols* ignoredSymbols) {
  if (!tree->isUserVariable()) {
    return false;
  }
  while (ignoredSymbols) {
    assert(ignoredSymbols->head);
    if (ignoredSymbols->head->treeIsIdenticalTo(tree)) {
      return true;
    }
    ignoredSymbols = reinterpret_cast<IgnoredSymbols*>(ignoredSymbols->tail);
  }
  return false;
}

// Call with a UserExpression tree
bool PrivateDeepSearch(const Tree* tree,
                       UserExpression::UserExpressionTrinaryTest test,
                       const DeepSearchParameters& parameters,
                       IgnoredSymbols* ignoredSymbols) {
  // - Step 1: Escape if should be ignored
  if (IsIgnoredSymbol(tree, ignoredSymbols)) {
    return false;
  }

  /* - Step 2: Clone the tree on the pool to test it as a UserExpression
   * WARNING: Do not keep the UserExpression alive after this point, as it could
   * clog the pool with clones when this function is called recursively on the
   * children. */
  OMG::Troolean testResult =
      test(UserExpression::Builder(tree), parameters.auxiliary);
  if (testResult == OMG::Troolean::True) {
    return true;
  } else if (testResult == OMG::Troolean::False) {
    return false;
  }
  assert(testResult == OMG::Troolean::Unknown);

  // - Step 3: Handle dependencies and store
  if (tree->isDep() || tree->isStore()) {
    return PrivateDeepSearch(tree->child(0), test, parameters, ignoredSymbols);
  }

  // - Step 4: Handle symbols
  SymbolicComputation replaceSymbols = parameters.symbolic;
  if (tree->isUserVariable() || tree->isUserFunction()) {
    assert(replaceSymbols == SymbolicComputation::ReplaceDefinedSymbols ||
           replaceSymbols == SymbolicComputation::ReplaceDefinedFunctions
           // We need only those cases for now
           || replaceSymbols == SymbolicComputation::KeepAllSymbols);

    if (replaceSymbols == SymbolicComputation::KeepAllSymbols ||
        (replaceSymbols == SymbolicComputation::ReplaceDefinedFunctions &&
         tree->isUserVariable())) {
      return false;
    }

    assert(replaceSymbols == SymbolicComputation::ReplaceDefinedSymbols ||
           tree->isUserFunction());

    // Undefined symbols must be preserved.
    Tree* clone = tree->cloneTree();
    Projection::DeepReplaceUserSymbol(
        clone, parameters.context, SymbolicComputation::ReplaceDefinedSymbols);
    // Keep the tree on the pool for future calls
    UserExpression eReplaced = UserExpression::Builder(clone);
    return !eReplaced.isUninitialized() &&
           PrivateDeepSearch(eReplaced.tree(), test,
                             {.context = parameters.context,
                              .symbolic = SymbolicComputation::KeepAllSymbols,
                              .auxiliary = parameters.auxiliary},
                             ignoredSymbols);
  }

  /* - Step 5: Test children
   *
   * TODO_PCJ: This is highly ineffective : each child of the tree is
   * cloned on the pool to be deepSearched. We do so so that
   * UserExpressionTrinaryTest can use Expression API.
   *
   * This is partially mitigated by the fact that the UserExpression are
   * not kept when going through children, so that not all the clones are
   * alive at the same time. So the memory usage shouldn't be a problem, but
   * timewise it is still a lot of useless clones.
   */
  bool isParametered = tree->isParametric();
  for (IndexedChild<const Tree*> child : tree->indexedChildren()) {
    if (isParametered && child.index == Parametric::k_variableIndex) {
      continue;
    }
    bool matches;
    if (isParametered && child.index == Parametric::FunctionIndex(tree)) {
      const Tree* symbolTree = tree->child(Parametric::k_variableIndex);
      IgnoredSymbols updatedIgnoredSymbols = {.head = symbolTree,
                                              .tail = ignoredSymbols};
      matches =
          PrivateDeepSearch(child, test, parameters, &updatedIgnoredSymbols);
    } else {
      matches = PrivateDeepSearch(child, test, parameters, ignoredSymbols);
    }
    if (matches) {
      return true;
    }
  }
  return false;
}

bool UserExpression::deepSearch(UserExpressionTrinaryTest test,
                                const DeepSearchParameters& parameters) const {
  return PrivateDeepSearch(tree(), test, parameters, nullptr);
}

bool UserExpression::deepSearch(UserExpressionBinaryTest test,
                                const DeepSearchParameters& parameters) const {
  struct Pack {
    UserExpressionBinaryTest* test;
    void* auxiliary;
  };
  UserExpressionTrinaryTest ternary = [](const UserExpression& e, void* pack) {
    UserExpressionBinaryTest* trueTest =
        static_cast<UserExpressionBinaryTest*>(static_cast<Pack*>(pack)->test);
    return (*trueTest)(e, static_cast<Pack*>(pack)->auxiliary)
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  };
  Pack pack{&test, parameters.auxiliary};
  return deepSearch(ternary, {.context = parameters.context,
                              .symbolic = parameters.symbolic,
                              .auxiliary = &pack});
}

bool UserExpression::deepSearch(
    UserExpressionSimpleTest test,
    const SimpleDeepSearchParameters& parameters) const {
  UserExpressionTrinaryTest ternary = [](const UserExpression& e, void* pack) {
    UserExpressionSimpleTest* trueTest =
        static_cast<UserExpressionSimpleTest*>(pack);
    return (*trueTest)(e) ? OMG::Troolean::True : OMG::Troolean::Unknown;
  };
  return deepSearch(ternary, {.context = parameters.context,
                              .symbolic = parameters.symbolic,
                              .auxiliary = &test});
}

bool UserExpression::deepSearch(
    NonStaticSimpleTest test,
    const SimpleDeepSearchParameters& parameters) const {
  UserExpressionTrinaryTest ternary = [](const UserExpression& e, void* pack) {
    NonStaticSimpleTest* trueTest = static_cast<NonStaticSimpleTest*>(pack);
    return (e.**trueTest)() ? OMG::Troolean::True : OMG::Troolean::Unknown;
  };
  return deepSearch(ternary, {.context = parameters.context,
                              .symbolic = parameters.symbolic,
                              .auxiliary = &test});
}

bool UserExpression::hasUnit(bool ignoreAngleUnits, bool* hasAngleUnits,
                             bool replaceSymbols,
                             const SymbolContext& symbolContext) const {
  if (hasAngleUnits) {
    *hasAngleUnits = false;
  }
  struct Pack {
    bool ignoreAngleUnits;
    bool* hasAngleUnits;
  };
  Pack pack{ignoreAngleUnits, hasAngleUnits};
  return deepSearch(
      [](const UserExpression& e, void* arg) {
        Pack* pack = static_cast<Pack*>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool* hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.tree()->isUnitOrPhysicalConstant() &&
                (!pack->ignoreAngleUnits || !isAngleUnit));
      },
      {.context = symbolContext,
       .symbolic = replaceSymbols ? SymbolicComputation::ReplaceDefinedSymbols
                                  : SymbolicComputation::KeepAllSymbols,
       .auxiliary = &pack});
}

bool UserExpression::isParsedNumber() const {
  const Internal::Tree* tree = this->tree();
  if (tree->isOpposite()) {
    tree = tree->child(0);
  }
  return tree->isInteger() || tree->isDecimal();
}

/* TODO: Find a better way to avoid duplication of Builder, clone, and
 *       cloneChildAtIndex methods with other types of expressions. */
UserExpression UserExpression::Builder(const Tree* tree) {
  PoolHandle p = BuildPoolHandleFromTree(tree);
  return static_cast<UserExpression&>(p);
}

UserExpression UserExpression::Builder(Tree* tree) {
  UserExpression result = Builder(const_cast<const Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

template UserExpression UserExpression::Builder<float>(float);
template UserExpression UserExpression::Builder<double>(double);
template UserExpression UserExpression::Builder<float>(Coordinate2D<float>);
template UserExpression UserExpression::Builder<double>(Coordinate2D<double>);
template UserExpression UserExpression::Builder<float>(
    PointOrRealScalar<float>);
template UserExpression UserExpression::Builder<double>(
    PointOrRealScalar<double>);
template float UserExpression::privateApproximateToRealScalar<float>(
    const UserParameters&, const ApproximationPolicy&) const;
template double UserExpression::privateApproximateToRealScalar<double>(
    const UserParameters&, const ApproximationPolicy&) const;
template float UserExpression::approximateToRealScalar<float>(
    const UserParameters&, const ApproximationPolicy&) const;
template double UserExpression::approximateToRealScalar<double>(
    const UserParameters&, const ApproximationPolicy&) const;
template float UserExpression::approximateUnitToRealScalar<float>(
    const UserParameters&, const ApproximationPolicy&) const;
template double UserExpression::approximateUnitToRealScalar<double>(
    const UserParameters&, const ApproximationPolicy&) const;
template bool UserExpression::hasDefinedComplexApproximation<float>(
    AngleUnit, ComplexFormat, const SymbolContext&, float*, float*) const;
template bool UserExpression::hasDefinedComplexApproximation<double>(
    AngleUnit, ComplexFormat, const SymbolContext&, double*, double*) const;
template float UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
    float>(const char*, const SimplificationParameters&);
template double UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
    double>(const char*, const SimplificationParameters&);

}  // namespace Poincare
