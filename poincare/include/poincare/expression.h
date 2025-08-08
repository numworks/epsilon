#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <omg/enums.h>
#include <poincare/comparison_operator.h>
#include <poincare/helpers/parser.h>
#include <poincare/layout.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_object.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/sign.h>
#include <poincare/src/expression/dimension_type.h>
#include <poincare/src/memory/block.h>
#include <poincare/src/memory/k_tree_concept.h>

#include <span>

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
// TODO_PCJ: Expose ProjectionContext
struct ProjectionContext;
}  // namespace Poincare::Internal

namespace Poincare {

class Expression;

/* Aliases used to specify a Expression's type. TODO_PCJ: split them into
 * actual classes to prevent casting one into another. */

// Expression : Can be applied to different types of Expressions
// Can be layoutted (Not projected)
using UserExpression = Expression;
// Can be approximated without context
using NoContextExpression = Expression;
// Must have been projected
using ProjectedExpression = NoContextExpression;
// Must have been systematic simplified
using SystemExpression = ProjectedExpression;
// Can depend on a Variable and has been prepared for approximation
using PreparedFunction = NoContextExpression;
// PreparedFunction with Scalar approximation
using PreparedFunctionScalar = PreparedFunction;
// PreparedFunction with Point approximation
using PreparedFunctionPoint = PreparedFunction;

class Dimension {
 public:
  Dimension(const Expression e, Context* context = nullptr);

  bool isScalar();
  bool isMatrix();
  bool isVector();
  bool isUnit();
  bool isBoolean();
  bool isPoint();

  bool isList();
  bool isListOfScalars();
  bool isListOfUnits();
  bool isListOfBooleans();
  bool isListOfPoints();
  bool isEmptyList();

  bool isPointOrListOfPoints();

 private:
  Internal::MatrixDimension m_matrixDimension;
  Internal::DimensionType m_type;
  bool m_isList;
  bool m_isValid;
  bool m_isEmptyList;
};

class ExpressionObject final : public PoolObject {
  friend class Expression;

 public:
  ExpressionObject(const Internal::Tree* tree, size_t treeSize);

  // PoolObject
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logObjectName(std::ostream& stream) const override {
    stream << "Expression";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context,
                      OMG::Base base = OMG::Base::Decimal,
                      bool linearMode = false) const;
  size_t serialize(std::span<char> buffer, bool compactMode,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const;

 private:
  // PCJ
  const Internal::Tree* tree() const;

  Internal::Block m_blocks[0];
};

class Expression : public PoolHandle {
  friend class ExpressionObject;

 public:
  Expression() : PoolHandle() {}
  Expression(const ExpressionObject* n) : PoolHandle(n) {}

  Expression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<Expression&>(clone);
  }
  bool isIdenticalTo(const Expression e) const;

  static Expression ExpressionFromAddress(const void* address, size_t size);

  /* Get a Tree from the storage, more efficient and safer than
   * ExpressionFromAddress.tree() because it points to the storage directly. */
  static const Internal::Tree* TreeFromAddress(const void* address);

  static UserExpression Parse(const Internal::Tree* layout,
                              const Context& context,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression Parse(const char* layout, const Context& context,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression ParseLatex(const char* latex, const Context& context,
                                   ParserHelper::ParsingParameters params = {});

  static UserExpression Create(const Internal::Tree* structure,
                               Internal::ContextTrees ctx);
  static SystemExpression CreateReduce(const Internal::Tree* structure,
                                       Internal::ContextTrees ctx);
  static SystemExpression CreateIntegralOfAbsOfDifference(
      SystemExpression lowerBound, SystemExpression upperBound,
      SystemExpression integrandA, SystemExpression integrandB);
  operator const Internal::Tree*() const { return tree(); }
  // Builders from value.
  static SystemExpression Builder(int32_t n);
  template <typename T>
  static SystemExpression Builder(T x);
  template <typename T>
  static SystemExpression Builder(Coordinate2D<T> point);
  template <typename T>
  static SystemExpression Builder(PointOrRealScalar<T> pointOrReal);

  static SystemExpression DecimalBuilderFromDouble(double v);
  static SystemExpression RationalBuilder(int32_t numerator,
                                          int32_t denominator);

  template <Internal::KTrees::KTreeConcept T>
  static Expression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  static Expression Builder(const Internal::Tree* tree);
  // Eat the tree
  static Expression Builder(Internal::Tree* tree);
  // Build the unit corresponding to the angleUnit preference.
  static UserExpression Builder(Preferences::AngleUnit angleUnit);
  // Build an expression of Undefined tree.
  static Expression Undefined();

  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : object()->tree();
  }
  Expression cloneChildAtIndex(int i) const;
  int numberOfDescendants(bool includeSelf) const;

  // The following two methods should be moved out of Expression's public
  // API.
  bool isOfType(std::initializer_list<Internal::AnyType> types) const;
  bool deepIsOfType(std::initializer_list<Internal::AnyType> types,
                    Context* context = nullptr) const;

  ExpressionObject* object() const {
    assert(identifier() != PoolObject::NoObjectIdentifier);
    return static_cast<ExpressionObject*>(PoolHandle::object());
  }

  /* Only on UserExpression. Expressions in parameters are outputs. The return
   * boolean indicates the reduction status (success or failure) */
  bool cloneAndSimplifyAndApproximate(
      UserExpression* simplifiedExpression,
      UserExpression* approximatedExpression,
      Internal::ProjectionContext& context) const;
  // Only on UserExpression
  UserExpression cloneAndSimplify(const Internal::ProjectionContext& context,
                                  bool* reductionFailure) const;
  /* This version does not warn if simplification fails. In case of failure the
   * initial expression is returned. */
  UserExpression cloneAndTrySimplify(
      const Internal::ProjectionContext& context) const;

  // Only on UserExpression
  SystemExpression cloneAndReduce(
      const Internal::ProjectionContext& projectionContext,
      bool* reductionFailure) const;
  // Only on UserExpression
  UserExpression cloneAndApproximate(
      Internal::ProjectionContext& context) const;

  // Only on SystemExpression. Expressions in parameters are outputs.
  void cloneAndBeautifyAndApproximate(
      UserExpression* beautifiedExpression,
      UserExpression* approximatedExpression,
      Internal::ProjectionContext& context) const;
  // Only on SystemExpression
  UserExpression cloneAndBeautify(Internal::ProjectionContext& context) const;
  // Only on SystemExpression. Replace symbol and reduce.
  SystemExpression cloneAndReplaceSymbolWithExpression(
      const char* symbolName, const SystemExpression& replaceSymbolWith,
      bool* reductionFailure, SymbolicComputation symbolic) const;

  SystemExpression getReducedDerivative(const char* symbolName,
                                        int derivationOrder = 1) const;
  /* Replace some UserSymbol into Var0 for
   * approximateToPointOrRealScalarWithValue Returns undef if the expression's
   * dimension is not point or scalar. If scalarsOnly = true, returns undef if
   * it's a point or a list. */
  PreparedFunction getPreparedFunction(const char* symbolName,
                                       bool scalarsOnly = false) const;
  // Approximate real scalar or unit
  template <typename T>
  T approximateToRealScalar(
      Preferences::AngleUnit angleUnit = Preferences::AngleUnit::None,
      Preferences::ComplexFormat complexFormat =
          Preferences::ComplexFormat::None,
      Context* context = nullptr) const;
  // Approximate to real scalar replacing Var0 with value.
  template <typename T>
  T approximateToRealScalarWithValue(T x, int listElement = -1) const;
  // Approximate to PointOrRealScalar replacing Var0 with value.
  template <typename T>
  PointOrRealScalar<T> approximateToPointOrRealScalarWithValue(T x) const;

  template <typename T>
  T approximateSystemToRealScalar() const;

  template <typename T>
  Coordinate2D<T> approximateToPoint() const;

  // Return SystemExpression with sorted approximated elements.
  template <typename T>
  SystemExpression approximateListAndSort() const;
  // Return SystemExpression with undef list elements or points removed.
  SystemExpression removeUndefAndComplexListElements() const;

  template <typename T>
  SystemExpression approximateSystemToTree() const;

  template <typename T>
  SystemExpression approximateUserToTree(
      Preferences::AngleUnit angleUnit,
      Preferences::ComplexFormat complexFormat, Context* context) const;

  /* Approximation Helper
   * Return NAN for all non real scalar expressions. */
  template <typename T>
  static T ParseAndSimplifyAndApproximateToRealScalar(
      const char* text, Context* context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbols);
  template <typename T>
  /* Return true when both real and imaginary approximation are defined and
   * imaginary part is not null. */
  bool hasDefinedComplexApproximation(Preferences::AngleUnit angleUnit,
                                      Preferences::ComplexFormat complexFormat,
                                      Context* context,
                                      T* returnRealPart = nullptr,
                                      T* returnImagPart = nullptr) const;
  bool isComplexScalar(
      Preferences::CalculationPreferences calculationPreferences,
      Context* context) const;
  bool involvesDiscontinuousFunction() const;
  template <typename T>
  bool isDiscontinuousOnInterval(T minBound, T maxBound) const;

  int polynomialDegree(const char* symbolName) const;
  /* Fills the table coefficients with the expressions of the first 3 polynomial
   * coefficients and returns the  polynomial degree. It is supposed to be
   * called on a reduced expression. coefficients has up to 3 entries.  */
  int getPolynomialReducedCoefficients(const char* symbolName,
                                       SystemExpression coefficients[],
                                       Context* context,
                                       Preferences::ComplexFormat complexFormat,
                                       Preferences::AngleUnit angleUnit,
                                       Preferences::UnitFormat unitFormat,
                                       SymbolicComputation symbolicComputation,
                                       bool keepDependencies = false) const;

  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context,
                      OMG::Base base = OMG::Base::Decimal,
                      bool linearMode = false) const;
  char* toLatex(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits, Context* context,
                bool withThousandsSeparator = false) const;

  /* TODO: detect when the buffer size was to small to hold the expression
   * serialization, and return an error code so that the caller can handle this
   * case. */
  size_t serialize(std::span<char> buffer, bool compactMode = false,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits =
                       PrintFloat::k_maxNumberOfSignificantDigits) const;

  bool replaceSymbolWithExpression(const UserExpression& symbol,
                                   const Expression& expression,
                                   bool onlySecondTerm = false);
  bool replaceSymbolWithUnknown(const UserExpression& symbol,
                                bool onlySecondTerm = false);
  bool replaceUnknownWithSymbol(CodePoint symbol);

  bool replaceSymbols(Poincare::Context* context,
                      SymbolicComputation symbolic =
                          SymbolicComputation::ReplaceDefinedSymbols);

  typedef OMG::Troolean (*ExpressionTrinaryTest)(const Expression e,
                                                 Context* context,
                                                 void* auxiliary);
  struct IgnoredSymbols {
    Expression* head;
    void* tail;
  };
  bool recursivelyMatches(ExpressionTrinaryTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr,
                          IgnoredSymbols* ignoredSymbols = nullptr) const;

  typedef bool (*ExpressionTest)(const Expression e, Context* context);
  bool recursivelyMatches(ExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*SimpleExpressionTest)(const Expression e);
  bool recursivelyMatches(SimpleExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (Expression::*NonStaticSimpleExpressionTest)() const;
  bool recursivelyMatches(NonStaticSimpleExpressionTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*ExpressionTestAuxiliary)(const Expression e, Context* context,
                                          void* auxiliary);
  bool recursivelyMatches(ExpressionTestAuxiliary test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr) const;

  Dimension dimension(Context* context = nullptr) const;

  // Sign of a SystemExpression
  Sign sign() const;

  // Simple bool properties
  bool isUndefined() const;
  bool isUndefinedOrNonReal() const;
  bool isNAry() const;
  bool isApproximate() const;
  bool isPlusOrMinusInfinity() const;
  bool isPercent() const;
  bool isSequence() const;
  bool isIntegral() const;
  bool isDiff() const;
  bool isParametric() const;
  bool isBoolean() const;
  bool isList() const;
  bool isUserSymbol() const;
  bool isUserFunction() const;
  bool isStore() const;
  bool isFactor() const;
  bool isPoint() const;
  bool isMatrix() const;
  bool isOfMatrixDimension() const;
  bool isNonReal() const;
  bool isOpposite() const;
  bool isDiv() const;
  bool isBasedInteger() const;
  bool isDep() const;
  bool isComparison() const;
  bool isEquality() const;
  bool isRational() const;
  // Return true if expression is a number, constant, inf or undef.
  bool isConstantNumber() const;
  // Return true if Decimal, Integer or Opposite of those.
  bool isParsedNumber() const;
  bool isPureAngleUnit() const;
  bool isVector() const;
  bool allChildrenAreUndefined() const;
  bool hasRandomNumber() const;
  bool hasRandomList() const;
  int numberOfChildren() const;

  // More complex bool properties
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false, Context* ctx = nullptr) const;
  bool isInRadians(Context* context) const;

  ComparisonJunior::Operator comparisonOperator() const;

 private:
  Expression privateCloneAndReduceOrSimplify(
      const Internal::ProjectionContext& context, bool beautify,
      bool* reductionFailure = nullptr) const;
};

class Infinity {
 public:
  static const char* k_infinityName;
  static const char* k_minusInfinityName;
};

}  // namespace Poincare

#endif
