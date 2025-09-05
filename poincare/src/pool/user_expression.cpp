#include <poincare/cas.h>
#include <poincare/helpers/layout.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/layout.h>
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

UserExpression UserExpression::Parse(const Tree* layout, const Context& context,
                                     ParserHelper::ParsingParameters params) {
  return UserExpression::Builder(Parser::Parse(layout, context, params));
}

UserExpression UserExpression::Parse(const char* string, const Context& context,
                                     ParserHelper::ParsingParameters params) {
  if (string[0] == 0) {
    return UserExpression();
  }
  Tree* layout = RackFromText(string);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result = Parse(layout, context, params);
  layout->removeTree();
  return result;
}

UserExpression UserExpression::ParseLatex(
    const char* latex, const Context& context,
    ParserHelper::ParsingParameters params) {
  Tree* layout = LatexParser::LatexToLayout(latex);
  if (!layout) {
    return UserExpression();
  }
  UserExpression result = Parse(layout, context, params);
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
    const Context& context) const {
  return SystemExpression::Builder(Approximation::ToTree<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, context)));
}

bool UserExpression::cloneAndSimplifyAndApproximate(
    UserExpression* simplifiedExpression,
    UserExpression* approximatedExpression,
    Internal::ProjectionContext& context) const {
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
    Internal::ProjectionContext& context) const {
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
    context.m_dimension = Internal::Dimension::Get(a);
    Beautification::DeepBeautify(a, context);
  }
  return UserExpression::Builder(a);
}

UserExpression UserExpression::cloneAndSimplify(
    const Internal::ProjectionContext& context, bool* reductionFailure) const {
  assert(reductionFailure);
  return privateCloneAndSimplify(context, reductionFailure);
}

UserExpression UserExpression::cloneAndTrySimplify(
    const Internal::ProjectionContext& context) const {
  return privateCloneAndSimplify(context, nullptr);
}

SystemExpression UserExpression::cloneAndReduce(
    const Internal::ProjectionContext& projectionContext,
    bool* reductionFailure) const {
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
    const Internal::ProjectionContext& context, bool* reductionFailure) const {
  assert(!isUninitialized());
  Tree* e = tree()->cloneTree();
  bool reductionSuccess = Simplification::Simplify(e, context, true);
  if (reductionFailure) {
    *reductionFailure = !reductionSuccess;
  }
  return UserExpression::Builder(e);
}

template <typename T>
T UserExpression::approximateToRealScalar(AngleUnit angleUnit,
                                          ComplexFormat complexFormat,
                                          const Context& context) const {
  static_assert(std::is_floating_point_v<T>);
  assert(Poincare::Dimension(*this, context).isScalar() ||
         Poincare::Dimension(*this, context).isUnit());
  return Approximation::To<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, context));
}

template <typename T>
T UserExpression::ParseAndSimplifyAndApproximateToRealScalar(
    const char* text, const Context& context, ComplexFormat complexFormat,
    AngleUnit angleUnit, SymbolicComputation symbolicComputation) {
  UserExpression exp = UserExpression::Parse(text, context);
  if (exp.isUninitialized()) {
    return NAN;
  }
  ProjectionContext ctx = ProjectionContext{.m_complexFormat = complexFormat,
                                            .m_angleUnit = angleUnit,
                                            .m_symbolic = symbolicComputation,
                                            .m_context = context};
  bool reductionFailure;
  exp = exp.cloneAndSimplify(ctx, &reductionFailure);
  assert(!exp.isUninitialized());
  Poincare::Dimension dimension = Poincare::Dimension(exp, context);
  if (!dimension.isScalar()) {
    return NAN;
  }
  return exp.approximateToRealScalar<T>(ctx.m_angleUnit, ctx.m_complexFormat,
                                        context);
}

template <typename T>
bool UserExpression::hasDefinedComplexApproximation(AngleUnit angleUnit,
                                                    ComplexFormat complexFormat,
                                                    const Context& context,
                                                    T* returnRealPart,
                                                    T* returnImagPart) const {
  if (complexFormat == ComplexFormat::Real ||
      !Internal::Dimension::IsNonListScalar(tree())) {
    return false;
  }
  std::complex<T> z = Approximation::ToComplex<T>(
      tree(),
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(angleUnit, complexFormat, context));
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
    const Context& context) const {
  ComplexFormat complexFormat =
      Preferences::UpdatedComplexFormatWithExpressionInput(
          calculationPreferences.complexFormat, *this, context);
  AngleUnit angleUnit = calculationPreferences.angleUnit;
  if (hasDefinedComplexApproximation<double>(angleUnit, complexFormat,
                                             context)) {
    assert(!hasUnit());
    return true;
  }
  return false;
}

Poincare::Layout UserExpression::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    const Context& context, OMG::Base base, bool linearMode) const {
  if (isUninitialized()) {
    return Poincare::Layout();
  }
  return Poincare::Layout::Builder(Layouter::LayoutExpression(
      tree()->cloneTree(), linearMode, false, numberOfSignificantDigits,
      floatDisplayMode, base));
}

char* UserExpression::toLatex(char* buffer, int bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits,
                              const Context& context,
                              bool withThousandsSeparator) const {
  return LatexParser::LayoutToLatex(
      Rack::From(
          createLayout(floatDisplayMode, numberOfSignificantDigits, context)
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
      tree()->cloneTree(), true, compactMode, numberOfSignificantDigits);
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

bool UserExpression::replaceSymbols(const Poincare::Context& context,
                                    SymbolicComputation symbolic) {
  /* Caution: must be called on an unprojected expression!
   * Indeed, the projection of the replacements has to be done at the same time
   * as the rest of the expression (otherwise inconsistencies could appear like
   * with random for example). */
  Tree* clone = tree()->cloneTree();
  bool didReplace = Projection::DeepReplaceUserNamed(clone, context, symbolic);
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
                                        const Context& context,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary) const {
  return recursivelyMatches(test, context, replaceSymbols, auxiliary, nullptr);
}

bool UserExpression::recursivelyMatches(UserExpressionTrinaryTest test,
                                        const Context& context,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary,
                                        IgnoredSymbols* ignoredSymbols) const {
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
    // Undefined symbols must be preserved.
    UserExpression e = clone();
    e.replaceSymbols(context, SymbolicComputation::ReplaceDefinedSymbols);
    return !e.isUninitialized() &&
           e.recursivelyMatches(test, context,
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

bool UserExpression::recursivelyMatches(
    ExpressionTest test, const Context& context,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary =
      [](const UserExpression e, const Context& context, void* auxiliary) {
        ExpressionTest* trueTest = static_cast<ExpressionTest*>(auxiliary);
        return (*trueTest)(e, context) ? OMG::Troolean::True
                                       : OMG::Troolean::Unknown;
      };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(
    SimpleExpressionTest test, const Context& context,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary =
      [](const UserExpression e, const Context& context, void* auxiliary) {
        SimpleExpressionTest* trueTest =
            static_cast<SimpleExpressionTest*>(auxiliary);
        return (*trueTest)(e) ? OMG::Troolean::True : OMG::Troolean::Unknown;
      };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(
    NonStaticSimpleExpressionTest test, const Context& context,
    SymbolicComputation replaceSymbols) const {
  UserExpressionTrinaryTest ternary =
      [](const UserExpression e, const Context& context, void* auxiliary) {
        NonStaticSimpleExpressionTest* trueTest =
            static_cast<NonStaticSimpleExpressionTest*>(auxiliary);
        return (e.**trueTest)() ? OMG::Troolean::True : OMG::Troolean::Unknown;
      };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool UserExpression::recursivelyMatches(ExpressionTestAuxiliary test,
                                        const Context& context,
                                        SymbolicComputation replaceSymbols,
                                        void* auxiliary) const {
  struct Pack {
    ExpressionTestAuxiliary* test;
    void* auxiliary;
  };
  UserExpressionTrinaryTest ternary = [](const UserExpression e,
                                         const Context& context, void* pack) {
    ExpressionTestAuxiliary* trueTest =
        static_cast<ExpressionTestAuxiliary*>(static_cast<Pack*>(pack)->test);
    return (*trueTest)(e, context, static_cast<Pack*>(pack)->auxiliary)
               ? OMG::Troolean::True
               : OMG::Troolean::Unknown;
  };
  Pack pack{&test, auxiliary};
  return recursivelyMatches(ternary, context, replaceSymbols, &pack);
}

bool UserExpression::hasUnit(bool ignoreAngleUnits, bool* hasAngleUnits,
                             bool replaceSymbols, const Context& ctx) const {
  if (hasAngleUnits) {
    *hasAngleUnits = false;
  }
  struct Pack {
    bool ignoreAngleUnits;
    bool* hasAngleUnits;
  };
  Pack pack{ignoreAngleUnits, hasAngleUnits};
  return recursivelyMatches(
      [](const UserExpression e, const Context& context, void* arg) {
        Pack* pack = static_cast<Pack*>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool* hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.tree()->isUnitOrPhysicalConstant() &&
                (!pack->ignoreAngleUnits || !isAngleUnit));
      },
      ctx,
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
    Internal::ProjectionContext&) const;
template UserExpression UserExpression::cloneAndApproximate<double>(
    Internal::ProjectionContext&) const;
template float UserExpression::approximateToRealScalar<float>(
    AngleUnit, ComplexFormat, const Context&) const;
template double UserExpression::approximateToRealScalar<double>(
    AngleUnit, ComplexFormat, const Context&) const;
template bool UserExpression::hasDefinedComplexApproximation<float>(
    AngleUnit, ComplexFormat, const Context&, float*, float*) const;
template bool UserExpression::hasDefinedComplexApproximation<double>(
    AngleUnit, ComplexFormat, const Context&, double*, double*) const;
template SystemExpression UserExpression::approximateUserToTree<float>(
    AngleUnit, ComplexFormat, const Context&) const;
template SystemExpression UserExpression::approximateUserToTree<double>(
    AngleUnit, ComplexFormat, const Context&) const;
template float
UserExpression::ParseAndSimplifyAndApproximateToRealScalar<float>(
    const char*, const Context&, ComplexFormat, AngleUnit, SymbolicComputation);
template double
UserExpression::ParseAndSimplifyAndApproximateToRealScalar<double>(
    const char*, const Context&, ComplexFormat, AngleUnit, SymbolicComputation);

}  // namespace Poincare
