#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/api.h>
#include <poincare/comparison_operator.h>
#include <poincare/old/computation_context.h>
#include <poincare/point_or_scalar.h>
#include <poincare/print_float.h>
#include <poincare/sign.h>
#include <poincare/src/expression/dimension_type.h>
#include <poincare/src/memory/block.h>
#include <poincare/src/memory/k_tree_concept.h>

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
// TODO_PCJ: Expose ProjectionContext
struct ProjectionContext;
}  // namespace Poincare::Internal

namespace Poincare {

/* Aliases used to specify a Expression's type. TODO_PCJ: split them into
 * actual classes to prevent casting one into another. */

// Can be applied to different types of Expressions
using NewExpression = Expression;
// Can be layoutted (Not projected)
using UserExpression = NewExpression;
// Can be approximated without context
using NoContextExpression = NewExpression;
// Must have been projected
using ProjectedExpression = NoContextExpression;
// Must have been systematic simplified
using SystemExpression = ProjectedExpression;
// Can depend on a Variable and has been prepared for approximation
using SystemFunction = NoContextExpression;
// SystemFunction with Scalar approximation
using SystemFunctionScalar = SystemFunction;
// SystemFunction with Point approximation
using SystemFunctionPoint = SystemFunction;

class Dimension {
 public:
  Dimension(const NewExpression e, Context* context = nullptr);

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
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Expression";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  // Properties

  template <typename T>
  SystemExpression approximateToTree(
      const ApproximationContext& approximationContext) const;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context,
                      OMG::Base base = OMG::Base::Decimal) const;
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

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
  Expression(const API::UserExpression& ue) { *this = Builder(ue.tree()); }

  NewExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<NewExpression&>(clone);
  }
  bool isIdenticalTo(const Expression e) const;

  static NewExpression ExpressionFromAddress(const void* address, size_t size);

  /* Get a Tree from the storage, more efficient and safer than
   * ExpressionFromAddress.tree() because it points to the storage directly. */
  static const Internal::Tree* TreeFromAddress(const void* address);

  static UserExpression Parse(const Internal::Tree* layout, Context* context,
                              bool addMissingParenthesis = true,
                              bool parseForAssignment = false);
  static UserExpression Parse(const char* layout, Context* context,
                              bool addMissingParenthesis = true,
                              bool parseForAssignment = false);
  static UserExpression ParseLatex(const char* latex, Context* context,
                                   bool addMissingParenthesis = true,
                                   bool parseForAssignment = false);

  static NewExpression Create(const Internal::Tree* structure,
                              Internal::ContextTrees ctx);
  static SystemExpression CreateReduce(const Internal::Tree* structure,
                                       Internal::ContextTrees ctx);
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
  static NewExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }
  static NewExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static NewExpression Builder(Internal::Tree* tree);

  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : object()->tree();
  }
  NewExpression cloneChildAtIndex(int i) const;
  int numberOfDescendants(bool includeSelf) const;

  // The following two methods should be moved out of Expression's public
  // API.
  bool isOfType(std::initializer_list<Internal::Type> types) const;
  bool deepIsOfType(std::initializer_list<Internal::Type> types,
                    Context* context = nullptr) const;

  ExpressionObject* object() const {
    assert(identifier() != PoolObject::NoNodeIdentifier &&
           !PoolHandle::object()->isGhost());
    return static_cast<ExpressionObject*>(PoolHandle::object());
  }

  void cloneAndSimplifyAndApproximate(UserExpression* simplifiedExpression,
                                      UserExpression* approximatedExpression,
                                      Internal::ProjectionContext* context,
                                      bool* reductionFailure = nullptr) const;
  UserExpression cloneAndSimplify(Internal::ProjectionContext* context,
                                  bool* reductionFailure = nullptr) const;
  SystemExpression cloneAndReduce(ReductionContext reductionContext,
                                  bool* reductionFailure = nullptr) const;
  UserExpression cloneAndBeautify(
      const ReductionContext& reductionContext) const;

  SystemExpression getReducedDerivative(const char* symbolName,
                                        int derivationOrder = 1) const;
  /* Replace some UserSymbol into Var0 for
   * approximateToPointOrRealScalarWithValue Returns undef if the expression's
   * dimension is not point or scalar. If scalarsOnly = true, returns undef if
   * it's a point or a list. */
  SystemFunction getSystemFunction(const char* symbolName,
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

  template <typename T>
  T approximateIntegralToRealScalar(const SystemExpression& lowerBound,
                                    const SystemExpression& upperBound) const;

  // Return SystemExpression with sorted approximated elements.
  template <typename T>
  SystemExpression approximateListAndSort() const;
  // Return SystemExpression with undef list elements or points removed.
  SystemExpression removeUndefListElements() const;

  template <typename T>
  SystemExpression approximateToTree(
      const ApproximationContext& approximationContext) const {
    return object()->approximateToTree<T>(approximationContext);
  }
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
  bool hasDefinedComplexApproximation(
      const ApproximationContext& approximationContext,
      T* returnRealPart = nullptr, T* returnImagPart = nullptr) const;
  bool isScalarComplex(
      Preferences::CalculationPreferences calculationPreferences,
      Context* context) const;
  bool involvesDiscontinuousFunction() const;
  template <typename T>
  bool isDiscontinuousOnInterval(T minBound, T maxBound) const;

  constexpr static int k_maxNumberOfVariables = 6;
  // TODO: factorize with k_maxPolynomialDegree from Polynomial
  constexpr static int k_maxPolynomialDegree = 3;
  // TODO: factorize with k_maxNumberOfPolynomialCoefficients from Polynomial
  constexpr static int k_maxNumberOfPolynomialCoefficients =
      k_maxPolynomialDegree + 1;
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
                      OMG::Base base = OMG::Base::Decimal) const;
  char* toLatex(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits, Context* context,
                bool withThousandsSeparator = false) const;

  /* TODO:
   * - Use same convention as strlcpy: return size of the source even if the
   * bufferSize was too small.*/
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits =
                       PrintFloat::k_maxNumberOfSignificantDigits) const;

  bool replaceSymbolWithExpression(const UserExpression& symbol,
                                   const NewExpression& expression,
                                   bool onlySecondTerm = false);
  bool replaceSymbolWithUnknown(const UserExpression& symbol,
                                bool onlySecondTerm = false);
  bool replaceUnknownWithSymbol(CodePoint symbol);

  bool replaceSymbols(Poincare::Context* context,
                      SymbolicComputation symbolic =
                          SymbolicComputation::ReplaceDefinedSymbols);

  typedef OMG::Troolean (*ExpressionTrinaryTest)(const NewExpression e,
                                                 Context* context,
                                                 void* auxiliary);
  struct IgnoredSymbols {
    NewExpression* head;
    void* tail;
  };
  bool recursivelyMatches(ExpressionTrinaryTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr,
                          IgnoredSymbols* ignoredSymbols = nullptr) const;

  typedef bool (*ExpressionTest)(const NewExpression e, Context* context);
  bool recursivelyMatches(ExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*SimpleExpressionTest)(const NewExpression e);
  bool recursivelyMatches(SimpleExpressionTest test, Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (Expression::*NonStaticSimpleExpressionTest)() const;
  bool recursivelyMatches(NonStaticSimpleExpressionTest test,
                          Context* context = nullptr,
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*ExpressionTestAuxiliary)(const NewExpression e,
                                          Context* context, void* auxiliary);
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
  bool isPureAngleUnit() const;
  bool allChildrenAreUndefined() const;
  bool hasRandomNumber() const;
  bool hasRandomList() const;

  // More complex bool properties
  bool isMatrix(Context* context = nullptr) const;
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false, Context* ctx = nullptr) const;
  bool isInRadians(Context* context) const;

  // This function can be used with recursivelyMatches
  static bool IsMatrix(const NewExpression e, Context* context = nullptr) {
    return e.isMatrix(context);
  }

  ComparisonJunior::Operator comparisonOperator() const;

#if 1
  /* TODO_PCJ: Remove those methods from PoolHandle once only Expression
   * remains. In the meantime, they are overriden there to assert false in case
   * they are still used. */
  void setParentIdentifier(uint16_t id) { assert(false); }
  void deleteParentIdentifier() { assert(false); }
  void deleteParentIdentifierInChildren() { assert(false); }
#endif

 private:
  SystemExpression cloneAndReduceAndBeautify(
      Internal::ProjectionContext* context, bool beautify,
      bool* reductionFailure = nullptr) const;
};

// TODO_PCJ: Actually implement methods. Assert its block type is Matrix
class Matrix final : public Expression {
 public:
  /* Cap the matrix's size for inverse and determinant computation.
   * TODO: Find another solution */
  constexpr static int k_maxNumberOfChildren = 100;
  static Matrix Builder();
  void setDimensions(int rows, int columns);
  bool isVector() const;
  int numberOfRows() const;
  int numberOfColumns() const;
  void addChildAtIndexInPlace(NewExpression t, int index,
                              int currentNumberOfChildren);
  NewExpression matrixChild(int i, int j);
  // rank returns -1 if the rank cannot be computed
  int rank(Context* context, bool forceCanonization = false);
};

class Point final : public Expression {
 public:
  static Point Builder(const Internal::Tree* x, const Internal::Tree* y);
  static Point Builder(const NewExpression x, const NewExpression y) {
    return Builder(x.tree(), y.tree());
  }
  Layout create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                        int significantDigits, Context* context) const;
};

// TODO_PCJ: Actually implement methods. Assert its block type is List
class List : public Expression {
 public:
  static List Builder();
  int numberOfChildren() const;

  void removeChildAtIndexInPlace(int i);
  void addChildAtIndexInPlace(NewExpression t, int index,
                              int currentNumberOfChildren);
};

class Unit final {
 public:
  // Build default unit for given angleUnit preference.
  static NewExpression Builder(Preferences::AngleUnit angleUnit);
  static bool IsPureAngleUnit(NewExpression expression, bool radianOnly);
  static bool HasAngleDimension(NewExpression expression);
};

class Undefined final : public Expression {
 public:
  static Undefined Builder();
  constexpr static const char* Name() { return "undef"; }
  constexpr static int NameSize() { return 6; }
};

class NonReal final : public Expression {
 public:
  static NonReal Builder();
  constexpr static const char* Name() { return "nonreal"; }
  constexpr static int NameSize() { return 8; }
};

class Infinity {
 public:
  static const char* k_infinityName;
  static const char* k_minusInfinityName;
};

}  // namespace Poincare

#endif
