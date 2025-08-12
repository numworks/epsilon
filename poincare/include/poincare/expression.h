#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <omg/enums.h>
#include <poincare/comparison_operator.h>
#include <poincare/context.h>
#include <poincare/helpers/parser.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>
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
// TODO: Expose ProjectionContext
struct ProjectionContext;
}  // namespace Poincare::Internal

namespace Poincare {

class Layout;
class Expression;
class UserExpression;
class SystemExpression;
class PreparedFunction;

/* Aliases used to specify a Expression's type. TODO: split them into
 * actual classes to prevent casting one into another. */
// PreparedFunction with Scalar approximation
using PreparedFunctionScalar = PreparedFunction;
// PreparedFunction with Point approximation
using PreparedFunctionPoint = PreparedFunction;

class Dimension {
 public:
  Dimension(const Expression e, const Context& context = EmptyContext{});

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

 private:
  const Internal::Tree* tree() const;

  Internal::Block m_blocks[0];
};

class Expression : public PoolHandle {
  friend class ExpressionObject;

 public:
  Expression() : PoolHandle() {}
  Expression(const ExpressionObject* n) : PoolHandle(n) {}

  /* General properties */

  int numberOfDescendants(bool includeSelf) const;
  int numberOfChildren() const;
  // Comparing trees. Expressions could be of a different derived class.
  bool isIdenticalTo(const Expression e) const;

  /* Boolean properties */
  // TODO: Specialize these properties if relevant to a derived class.
  bool allChildrenAreUndefined() const;
  bool hasRandomList() const;
  bool hasRandomNumber() const;
  bool isApproximate() const;
  bool isBasedInteger() const;
  bool isBoolean() const;
  bool isComparison() const;
  // Return true if expression is a number, constant, inf or undef.
  bool isConstantNumber() const;
  bool isDep() const;
  bool isDiff() const;
  bool isDiv() const;
  bool isEquality() const;
  bool isFactor() const;
  bool isInRadians(Context* context) const;
  bool isIntegral() const;
  bool isList() const;
  bool isMatrix() const;
  bool isNAry() const;
  bool isNonReal() const;
  bool isOfMatrixDimension() const;
  bool isOpposite() const;
  bool isPercent() const;
  bool isPlusOrMinusInfinity() const;
  bool isPoint() const;
  // Return true if Decimal, Integer or Opposite of those.
  bool isPureAngleUnit() const;
  bool isRational() const;
  bool isStore() const;
  bool isUndefined() const;
  bool isUndefinedOrNonReal() const;
  bool isUserFunction() const;
  bool isUserSymbol() const;
  bool isVector() const;

  /* Other properties */

  Dimension dimension(const Context& context = EmptyContext{}) const;
  // Only called on expressions that are comparisons
  Comparison::Operator comparisonOperator() const;

  /* Approximation Helper
   * Return NAN for all non real scalar expressions. */
  template <typename T>
  static T ParseAndSimplifyAndApproximateToRealScalar(
      const char* text, const Context& context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbols);

  /* Tree manipulation */
  // TODO: These methods should be protected

  /* Get a Tree from the storage, more efficient and safer than
   * ExpressionFromAddress.tree() because it points to the storage directly. */
  static const Internal::Tree* TreeFromAddress(const void* address);
  operator const Internal::Tree*() const { return tree(); }
  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : object()->tree();
  }

 protected:
  static Expression ExpressionFromAddress(const void* address, size_t size);
  static PoolHandle BuildPoolHandleFromTree(const Internal::Tree* tree);

 private:
  ExpressionObject* object() const {
    assert(identifier() != PoolObject::NoObjectIdentifier);
    return static_cast<ExpressionObject*>(PoolHandle::object());
  }
};

/* Expressions that can be laid out and have not been projected */
class UserExpression : public Expression {
 public:
  /* Static builders */

  static UserExpression ExpressionFromAddress(const void* address, size_t size);
  static UserExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static UserExpression Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static UserExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  template <typename T>
  static UserExpression Builder(Coordinate2D<T> point);
  template <typename T>
  static UserExpression Builder(PointOrRealScalar<T> pointOrReal);
  static UserExpression Builder(int32_t n);
  template <typename T>
  static UserExpression Builder(T x);
  // Build the unit corresponding to the angleUnit preference.
  static UserExpression Builder(Preferences::AngleUnit angleUnit);
  // Build an expression of Undefined tree.
  static UserExpression Undefined();

  static UserExpression Parse(const Internal::Tree* layout,
                              const Context& context,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression Parse(const char* layout, const Context& context,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression ParseLatex(const char* latex, const Context& context,
                                   ParserHelper::ParsingParameters params = {});

  static UserExpression Create(const Internal::Tree* structure,
                               Internal::ContextTrees ctx);

  /* General helpers */

  UserExpression cloneChildAtIndex(int i) const;
  UserExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<UserExpression&>(clone);
  }

  /* Non-const methods */
  // TODO: Rework them as cloneAnd* methods.
  bool replaceSymbolWithExpression(const UserExpression& symbol,
                                   const Expression& expression,
                                   bool onlySecondTerm = false);
  bool replaceSymbolWithUnknown(const UserExpression& symbol,
                                bool onlySecondTerm = false);
  bool replaceUnknownWithSymbol(CodePoint symbol);

  bool replaceSymbols(const Poincare::Context& context,
                      SymbolicComputation symbolic =
                          SymbolicComputation::ReplaceDefinedSymbols);

  /* UserExpression to UserExpression helpers */

  /* Expressions in parameters are outputs. The return boolean indicates the
   * reduction status (success or failure) */
  bool cloneAndSimplifyAndApproximate(
      UserExpression* simplifiedExpression,
      UserExpression* approximatedExpression,
      Internal::ProjectionContext& context) const;
  UserExpression cloneAndSimplify(const Internal::ProjectionContext& context,
                                  bool* reductionFailure) const;
  /* This version does not warn if simplification fails. In case of failure the
   * initial expression is returned. */
  UserExpression cloneAndTrySimplify(
      const Internal::ProjectionContext& context) const;
  template <typename T>
  UserExpression cloneAndApproximate(
      Internal::ProjectionContext& context) const;

  /* Other helpers */

  SystemExpression cloneAndReduce(
      const Internal::ProjectionContext& projectionContext,
      bool* reductionFailure) const;
  template <typename T>
  SystemExpression approximateUserToTree(
      Preferences::AngleUnit angleUnit,
      Preferences::ComplexFormat complexFormat, Context* context) const;
  // Approximate real scalar or unit
  template <typename T>
  T approximateToRealScalar(
      Preferences::AngleUnit angleUnit = Preferences::AngleUnit::None,
      Preferences::ComplexFormat complexFormat =
          Preferences::ComplexFormat::None,
      const Context& context = EmptyContext{}) const;

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

  /* Properties getters */

  /* Return true when both real and imaginary approximation are defined and
   * imaginary part is not null. */
  template <typename T>
  bool hasDefinedComplexApproximation(Preferences::AngleUnit angleUnit,
                                      Preferences::ComplexFormat complexFormat,
                                      const Context& context,
                                      T* returnRealPart = nullptr,
                                      T* returnImagPart = nullptr) const;
  bool isComplexScalar(
      Preferences::CalculationPreferences calculationPreferences,
      const Context& context) const;
  bool isParsedNumber() const;
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false, Context* ctx = nullptr) const;

  /* TODO: The following two methods should be made private or deleted to hide
   *       Internal node types. */
  bool isOfType(std::initializer_list<Internal::AnyType> types) const;
  bool deepIsOfType(std::initializer_list<Internal::AnyType> types,
                    Context* context = nullptr) const;

  /* recursivelyMatches */

  typedef OMG::Troolean (*UserExpressionTrinaryTest)(const UserExpression e,
                                                     Context* context,
                                                     void* auxiliary);
  bool recursivelyMatches(UserExpressionTrinaryTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr) const;

  typedef bool (*ExpressionTest)(const UserExpression e, Context* context);
  bool recursivelyMatches(ExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*SimpleExpressionTest)(const UserExpression e);
  bool recursivelyMatches(SimpleExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (UserExpression::*NonStaticSimpleExpressionTest)() const;
  bool recursivelyMatches(NonStaticSimpleExpressionTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*ExpressionTestAuxiliary)(const UserExpression e,
                                          Context* context, void* auxiliary);
  bool recursivelyMatches(ExpressionTestAuxiliary test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr) const;

 private:
  struct IgnoredSymbols {
    UserExpression* head;
    void* tail;
  };
  static bool IsIgnoredSymbol(const UserExpression* e,
                              UserExpression::IgnoredSymbols* ignoredSymbols);
  bool recursivelyMatches(UserExpressionTrinaryTest test, Context* context,
                          SymbolicComputation replaceSymbols, void* auxiliary,
                          IgnoredSymbols* ignoredSymbols) const;

  UserExpression privateCloneAndSimplify(
      const Internal::ProjectionContext& context,
      bool* reductionFailure = nullptr) const;
};

/* Expressions that have been projected and systematic reduced. */
class SystemExpression : public Expression {
 public:
  /* Static builders */
  static SystemExpression ExpressionFromAddress(const void* address,
                                                size_t size);
  static SystemExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static SystemExpression Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static SystemExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  static SystemExpression Builder(int32_t n);
  template <typename T>
  static SystemExpression Builder(T x);
  template <typename T>
  static SystemExpression Builder(Coordinate2D<T> point);
  template <typename T>
  static SystemExpression Builder(PointOrRealScalar<T> pointOrReal);
  // Build an expression of Undefined tree.
  static SystemExpression Undefined();

  static SystemExpression DecimalBuilderFromDouble(double v);
  static SystemExpression RationalBuilder(int32_t numerator,
                                          int32_t denominator);
  static SystemExpression CreateReduce(const Internal::Tree* structure,
                                       Internal::ContextTrees ctx);
  static SystemExpression CreateIntegralOfAbsOfDifference(
      SystemExpression lowerBound, SystemExpression upperBound,
      SystemExpression integrandA, SystemExpression integrandB);

  /* General helpers */
  SystemExpression cloneChildAtIndex(int i) const;
  SystemExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<SystemExpression&>(clone);
  }

  /* SystemExpression to SystemExpression helpers */
  template <typename T>
  SystemExpression approximateSystemToTree() const;
  // Replace symbol and reduce.
  SystemExpression cloneAndReplaceSymbolWithExpression(
      const char* symbolName, const SystemExpression& replaceSymbolWith,
      bool* reductionFailure, SymbolicComputation symbolic) const;
  SystemExpression getReducedDerivative(const char* symbolName,
                                        int derivationOrder = 1) const;
  // Return SystemExpression with sorted approximated elements.
  template <typename T>
  SystemExpression approximateListAndSort() const;
  // Return SystemExpression with undef list elements or points removed.
  SystemExpression removeUndefAndComplexListElements() const;

  /* Other helpers */

  // Expressions in parameters are outputs.
  void cloneAndBeautifyAndApproximate(
      UserExpression* beautifiedExpression,
      UserExpression* approximatedExpression,
      Internal::ProjectionContext& context) const;
  UserExpression cloneAndBeautify(Internal::ProjectionContext& context) const;
  /* Replace some UserSymbol into Var0 for
   * approximateToPointOrRealScalarWithValue. Returns undef if the expression's
   * dimension is not point or scalar. If scalarsOnly = true, returns undef if
   * it's a point or a list. */
  PreparedFunction getPreparedFunction(const char* symbolName,
                                       bool scalarsOnly = false) const;
  template <typename T>
  T approximateSystemToRealScalar() const;
  template <typename T>
  Coordinate2D<T> approximateToPoint() const;

  /* Properties getters */

  Sign sign() const;
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
};

/* Expressions that have been prepared for approximation. It can depend on a
 * Variable of index 0 called Var0. */
class PreparedFunction : public Expression {
 public:
  /* Static builders */
  static PreparedFunction Builder(const Internal::Tree* tree);
  // Eat the tree
  static PreparedFunction Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static PreparedFunction Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }

  /* General helpers */
  PreparedFunction cloneChildAtIndex(int i) const;
  PreparedFunction clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<PreparedFunction&>(clone);
  }

  /* Other helpers */
  template <typename T>
  Coordinate2D<T> approximateToPoint() const;
  // Approximate to real scalar replacing Var0 with value.
  template <typename T>
  T approximateToRealScalarWithValue(T x, int listElement = -1) const;
  // Approximate to PointOrRealScalar replacing Var0 with value.
  template <typename T>
  PointOrRealScalar<T> approximateToPointOrRealScalarWithValue(T x) const;

  /* Properties getters */
  bool involvesDiscontinuousFunction() const;
  template <typename T>
  bool isDiscontinuousOnInterval(T minBound, T maxBound) const;
  bool hasSequences() const;
  bool approximationBasedOnCostlyAlgorithms() const;
};

}  // namespace Poincare

#endif
