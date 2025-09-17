#include <poincare/cas.h>
#include <poincare/helpers/layout.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/layout.h>
#include <poincare/projection_context.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
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
  Expression e = Expression::ExpressionFromAddress(address, size);
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
  for (UserExpression expr : expressions) {
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

template <typename T>
SystemExpression UserExpression::approximateUserToTree(
    AngleUnit angleUnit, ComplexFormat complexFormat,
    const SymbolContext& symbolContext) const {
  return SystemExpression::Builder(Approximation::ToTree<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, symbolContext)));
}

bool UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression, ProjectionContext& context) const {
  // Step 1: simplify
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  bool reductionFailure = false;
  *simplifiedExpression = cloneAndSimplify(context, &reductionFailure);
  assert(!simplifiedExpression->isUninitialized());
  // Step 2: approximate
  assert(approximatedExpression && approximatedExpression->isUninitialized());
  *approximatedExpression =
      simplifiedExpression->cloneAndApproximate<double>(context);
  return reductionFailure;
}

template <typename T>
UserExpression UserExpression::cloneAndApproximate(
    ProjectionContext& context) const {
  Approximation::Context approxCtx(context.m_angleUnit, context.m_complexFormat,
                                   context.m_context);
  Tree* a;
  if (CAS::Enabled()) {
    a = tree()->cloneTree();
    /* We are using ApproximateAndReplaceEveryScalar to approximate
     * expressions with symbols such as π*x → 3.14*x. */
    Approximation::ApproximateAndReplaceEveryScalar<T>(a, approxCtx);
  } else {
    // Note: The non-beautified expression could be approximated instead.
    a = Approximation::ToTree<T>(
        tree(),
        Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                  .projectLocalVariables = true},
        approxCtx);
    Beautification::DeepBeautify(a, context, Internal::Dimension::Get(a));
  }
  return UserExpression::Builder(a);
}

UserExpression UserExpression::cloneAndSimplify(
    const ProjectionContext& context, bool* reductionFailure) const {
  assert(reductionFailure);
  return privateCloneAndSimplify(context, reductionFailure);
}

UserExpression UserExpression::cloneAndTrySimplify(
    const ProjectionContext& context) const {
  return privateCloneAndSimplify(context, nullptr);
}

SystemExpression UserExpression::cloneAndReduce(
    const ProjectionContext& projectionContext, bool* reductionFailure) const {
  assert(reductionFailure);
  Tree* e = tree()->cloneTree();
  bool reductionSuccess = Simplification::Simplify(e, projectionContext, false);
  if (reductionFailure) {
    /* TODO: In case of reductionFailure, returned expression is actually a
     * UserExpression ([this]). */
    *reductionFailure = !reductionSuccess;
  }
  return SystemExpression::Builder(e);
}

UserExpression UserExpression::privateCloneAndSimplify(
    const ProjectionContext& context, bool* reductionFailure) const {
  assert(!isUninitialized());
  Tree* e = tree()->cloneTree();
  bool reductionSuccess = Simplification::Simplify(e, context, true);
  if (reductionFailure) {
    *reductionFailure = !reductionSuccess;
  }
  return UserExpression::Builder(e);
}

template <typename T>
T UserExpression::approximateToRealScalar(
    AngleUnit angleUnit, ComplexFormat complexFormat,
    const SymbolContext& symbolContext) const {
  static_assert(std::is_floating_point_v<T>);
  assert(Poincare::Dimension(*this, symbolContext).isScalar() ||
         Poincare::Dimension(*this, symbolContext).isUnit());
  return Approximation::To<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, symbolContext));
}

template <typename T>
T UserExpression::ParseAndSimplifyAndApproximateToRealScalar(
    const char* text, const SymbolContext& symbolContext,
    ComplexFormat complexFormat, AngleUnit angleUnit,
    SymbolicComputation symbolicComputation) {
  UserExpression exp = UserExpression::Parse(text, symbolContext);
  if (exp.isUninitialized()) {
    return NAN;
  }
  ProjectionContext ctx = ProjectionContext{.m_complexFormat = complexFormat,
                                            .m_angleUnit = angleUnit,
                                            .m_symbolic = symbolicComputation,
                                            .m_context = symbolContext};
  bool reductionFailure;
  exp = exp.cloneAndSimplify(ctx, &reductionFailure);
  assert(!exp.isUninitialized());
  Poincare::Dimension dimension = Poincare::Dimension(exp, symbolContext);
  if (!dimension.isScalar()) {
    return NAN;
  }
  return exp.approximateToRealScalar<T>(ctx.m_angleUnit, ctx.m_complexFormat,
                                        symbolContext);
}

ComplexFormat UserExpression::preferedComplexFormat(
    ComplexFormat complexFormat, const SymbolContext& symbolContext,
    SymbolicComputation replaceSymbols) const {
  ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_symbolic = replaceSymbols,
      .m_context = symbolContext,
  };
  Internal::Projection::UpdateComplexFormatWithExpressionInput(
      tree(), projectionContext);
  return projectionContext.m_complexFormat;
}

bool UserExpression::updateProjectionContextWithPreferedComplexFormat(
    ProjectionContext& context) const {
  return Internal::Projection::UpdateComplexFormatWithExpressionInput(tree(),
                                                                      context);
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
  std::complex<T> z = Approximation::ToComplex<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, symbolContext));
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
  ComplexFormat complexFormat = preferedComplexFormat(
      calculationPreferences.complexFormat, symbolContext);
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
    const SymbolContext& symbolContext, OMG::Base base, bool linearMode) const {
  if (isUninitialized()) {
    return Poincare::Layout();
  }
  return Poincare::Layout::Builder(Layouter::LayoutExpression(
      tree(), {.symbolContext = symbolContext,
               .layouterMode =
                   linearMode ? LayouterMode::Linear : LayouterMode::Natural,
               .numberOfSignificantDigits = numberOfSignificantDigits,
               .floatMode = floatDisplayMode,
               .base = base}));
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

bool UserExpression::replaceSymbolWithExpression(const UserExpression& symbol,
                                                 const Expression& expression,
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
    *this = UserExpression::Builder(result);
    return true;
  }
  result->removeTree();
  return false;
}

bool UserExpression::replaceSymbolWithUnknown(const UserExpression& symbol,
                                              bool onlySecondTerm) {
  return replaceSymbolWithExpression(symbol, SymbolHelper::SystemSymbol(),
                                     onlySecondTerm);
}

bool UserExpression::replaceUnknownWithSymbol(CodePoint symbol) {
  return replaceSymbolWithExpression(SymbolHelper::SystemSymbol(),
                                     SymbolHelper::BuildSymbol(symbol));
}

bool UserExpression::replaceSymbols(
    const Poincare::SymbolContext& symbolContext,
    SymbolicComputation symbolic) {
  /* Caution: must be called on an unprojected expression!
   * Indeed, the projection of the replacements has to be done at the same time
   * as the rest of the expression (otherwise inconsistencies could appear like
   * with random for example). */
  Tree* clone = tree()->cloneTree();
  bool didReplace =
      Projection::DeepReplaceUserNamed(clone, symbolContext, symbolic);
  *this = UserExpression::Builder(clone);
  return didReplace;
}

bool UserExpression::IsIgnoredSymbol(
    const UserExpression* e, UserExpression::IgnoredSymbols* ignoredSymbols) {
  if (!e->tree()->isUserSymbol()) {
    return false;
  }
  while (ignoredSymbols) {
    assert(ignoredSymbols->head);
    if (ignoredSymbols->head->isIdenticalTo(*e)) {
      return true;
    }
    ignoredSymbols =
        reinterpret_cast<UserExpression::IgnoredSymbols*>(ignoredSymbols->tail);
  }
  return false;
}

bool UserExpression::recursivelyMatches(UserExpressionTrinaryTest test,
                                        const SymbolContext& symbolContext,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary) const {
  return recursivelyMatches(test, symbolContext, replaceSymbols, auxiliary,
                            nullptr);
}

bool UserExpression::recursivelyMatches(UserExpressionTrinaryTest test,
                                        const SymbolContext& symbolContext,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary,
                                        IgnoredSymbols* ignoredSymbols) const {
  if (IsIgnoredSymbol(this, ignoredSymbols)) {
    return false;
  }
  OMG::Troolean testResult = test(*this, symbolContext, auxiliary);
  if (testResult == OMG::Troolean::True) {
    return true;
  } else if (testResult == OMG::Troolean::False) {
    return false;
  }
  assert(testResult == OMG::Troolean::Unknown && !isUninitialized());

  // Handle dependencies, store, symbols and functions
  if (tree()->isDep() || tree()->isStore()) {
    return cloneChildAtIndex(0).recursivelyMatches(
        test, symbolContext, replaceSymbols, auxiliary, ignoredSymbols);
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
    // Undefined symbols must be preserved.
    UserExpression e = clone();
    e.replaceSymbols(symbolContext, SymbolicComputation::ReplaceDefinedSymbols);
    return !e.isUninitialized() &&
           e.recursivelyMatches(test, symbolContext,
                                SymbolicComputation::KeepAllSymbols, auxiliary,
                                ignoredSymbols);
  }

  /* TODO_PCJ: This is highly ineffective : each child of the tree is cloned on
   * the pool to be recursivelyMatched. We do so so that
   * UserExpressionTrinaryTest can use Expression API. */
  const int childrenCount = numberOfChildren();

  bool isParametered = tree()->isParametric();
  // Run loop backwards to find lists and matrices quicker in NAry expressions
  for (int i = childrenCount - 1; i >= 0; i--) {
    if (isParametered && i == Parametric::k_variableIndex) {
      continue;
    }
    UserExpression childToAnalyze = cloneChildAtIndex(i);
    bool matches;
    if (isParametered && i == Parametric::FunctionIndex(tree())) {
      UserExpression symbolExpr =
          cloneChildAtIndex(Parametric::k_variableIndex);
      IgnoredSymbols updatedIgnoredSymbols = {.head = &symbolExpr,
                                              .tail = ignoredSymbols};
      matches =
          childToAnalyze.recursivelyMatches(test, symbolContext, replaceSymbols,
                                            auxiliary, &updatedIgnoredSymbols);
    } else {
      matches = childToAnalyze.recursivelyMatches(
          test, symbolContext, replaceSymbols, auxiliary, ignoredSymbols);
    }
    if (matches) {
      return true;
    }
  }
  return false;
}

bool UserExpression::recursivelyMatches(
    ExpressionTest test, const SymbolContext& symbolContext,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary = [](const UserExpression e,
                                         const SymbolContext& symbolContext,
                                         void* auxiliary) {
    ExpressionTest* trueTest = static_cast<ExpressionTest*>(auxiliary);
    return (*trueTest)(e, symbolContext) ? OMG::Troolean::True
                                         : OMG::Troolean::Unknown;
  };
  return recursivelyMatches(ternary, symbolContext, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(
    SimpleExpressionTest test, const SymbolContext& symbolContext,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary = [](const UserExpression e,
                                         const SymbolContext& symbolContext,
                                         void* auxiliary) {
    SimpleExpressionTest* trueTest =
        static_cast<SimpleExpressionTest*>(auxiliary);
    return (*trueTest)(e) ? OMG::Troolean::True : OMG::Troolean::Unknown;
  };
  return recursivelyMatches(ternary, symbolContext, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(
    NonStaticSimpleExpressionTest test, const SymbolContext& symbolContext,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary = [](const UserExpression e,
                                         const SymbolContext& symbolContext,
                                         void* auxiliary) {
    NonStaticSimpleExpressionTest* trueTest =
        static_cast<NonStaticSimpleExpressionTest*>(auxiliary);
    return (e.**trueTest)() ? OMG::Troolean::True : OMG::Troolean::Unknown;
  };
  return recursivelyMatches(ternary, symbolContext, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(ExpressionTestAuxiliary test,
                                        const SymbolContext& symbolContext,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary) const {
  struct Pack {
    ExpressionTestAuxiliary* test;
    void* auxiliary;
  };
  UserExpressionTrinaryTest ternary = [](const UserExpression e,
                                         const SymbolContext& symbolContext,
                                         void* pack) {
    ExpressionTestAuxiliary* trueTest =
        static_cast<ExpressionTestAuxiliary*>(static_cast<Pack*>(pack)->test);
    return (*trueTest)(e, symbolContext, static_cast<Pack*>(pack)->auxiliary)
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  };
  Pack pack{&test, auxiliary};
  return recursivelyMatches(ternary, symbolContext, replaceSymbols, &pack);
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
  return recursivelyMatches(
      [](const UserExpression e, const SymbolContext& symbolContext,
         void* arg) {
        Pack* pack = static_cast<Pack*>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool* hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.tree()->isUnitOrPhysicalConstant() &&
                (!pack->ignoreAngleUnits || !isAngleUnit));
      },
      symbolContext,
      replaceSymbols ? SymbolicComputation::ReplaceDefinedSymbols
                     : SymbolicComputation::KeepAllSymbols,
      &pack);
}

bool UserExpression::isParsedNumber() const {
  const Internal::Tree* tree = this->tree();
  if (tree->isOpposite()) {
    tree = tree->nextNode();
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
template UserExpression UserExpression::cloneAndApproximate<float>(
    ProjectionContext&) const;
template UserExpression UserExpression::cloneAndApproximate<double>(
    ProjectionContext&) const;
template float UserExpression::approximateToRealScalar<float>(
    AngleUnit, ComplexFormat, const SymbolContext&) const;
template double UserExpression::approximateToRealScalar<double>(
    AngleUnit, ComplexFormat, const SymbolContext&) const;
template bool UserExpression::hasDefinedComplexApproximation<float>(
    AngleUnit, ComplexFormat, const SymbolContext&, float*, float*) const;
template bool UserExpression::hasDefinedComplexApproximation<double>(
    AngleUnit, ComplexFormat, const SymbolContext&, double*, double*) const;
template SystemExpression UserExpression::approximateUserToTree<float>(
    AngleUnit, ComplexFormat, const SymbolContext&) const;
template SystemExpression UserExpression::approximateUserToTree<double>(
    AngleUnit, ComplexFormat, const SymbolContext&) const;
template float UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
    float>(const char*, const SymbolContext&, ComplexFormat, AngleUnit,
           SymbolicComputation);
template double UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
    double>(const char*, const SymbolContext&, ComplexFormat, AngleUnit,
            SymbolicComputation);

}  // namespace Poincare
