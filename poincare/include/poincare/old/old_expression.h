#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <ion/storage/file_system.h>
#include <poincare/print_float.h>

#include <utility>

#include "aliases_list.h"
#include "complex.h"
#include "coordinate_2D.h"
#include "expression_node.h"
#include "layout.h"
#include "pool_handle.h"
#include "preferences.h"

namespace Poincare {

class Context;

class OExpression : public PoolHandle {
  friend class AbsoluteValue;
  friend class Addition;
  friend class AdditionNode;
  friend class ArcCosecant;
  friend class ArcCosine;
  friend class ArcCotangent;
  friend class ArcSecant;
  friend class ArcSine;
  friend class ArcTangent;
  friend class Arithmetic;
  friend class BasedInteger;
  friend class BinomialCoefficient;
  friend class Ceiling;
  friend class Comparison;
  friend class ComplexArgument;
  friend class ComplexCartesian;
  friend class ComplexHelper;
  template <typename T>
  friend class ComplexNode;
  friend class Conjugate;
  friend class Constant;
  friend class Cosecant;
  friend class Cosine;
  friend class Cotangent;
  friend class Decimal;
  friend class Dependency;
  friend class DependencyNode;
  friend class Derivative;
  friend class DerivativeNode;
  friend class Determinant;
  friend class Dimension;
  friend class DistributionDispatcher;
  friend class Division;
  friend class DivisionQuotient;
  friend class DivisionRemainder;
  friend class EqualNode;
  template <typename T>
  friend class ExceptionExpressionNode;
  friend class ExpressionNode;
  friend class Factor;
  friend class Factorial;
  template <typename T>
  friend class FloatList;
  friend class Floor;
  friend class FracPart;
  friend class Function;
  friend class FunctionNode;
  friend class GlobalContext;
  friend class GreatCommonDivisor;
  friend class HyperbolicTrigonometricFunction;
  friend class ImaginaryPart;
  friend class Integer;
  friend class Integral;
  friend class IntegralNode;
  friend class InverseMethod;
  friend class LeastCommonMultiple;
  friend class JuniorExpressionNode;
  friend class OList;
  friend class ListFunctionWithOneOrTwoParametersNode;
  friend class ListMean;
  friend class ListMedian;
  friend class ListSampleStandardDeviation;
  friend class ListSequence;
  friend class ListStandardDeviation;
  friend class ListVariance;
  friend class Logarithm;
  friend class LogarithmNode;
  friend class LogicalOperatorNode;
  friend class OMatrix;
  friend class MatrixEchelonForm;
  friend class MatrixIdentity;
  friend class MatrixInverse;
  friend class MatrixNode;
  friend class MatrixReducedRowEchelonForm;
  friend class MatrixRowEchelonForm;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class MixedFraction;
  friend class Multiplication;
  friend class MultiplicationNode;
  friend class NAryExpressionNode;
  friend class NAryExpression;
  friend class NAryInfixExpressionNode;
  friend class NaperianLogarithm;
  friend class NaperianLogarithmNode;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class ParameteredExpression;
  friend class ParameteredExpressionNode;
  friend class Parenthesis;
  friend class PercentAddition;
  friend class PercentSimple;
  friend class PermuteCoefficient;
  friend class PiecewiseOperator;
  friend class PolarConic;
  friend class Power;
  friend class PowerNode;
  friend class Product;
  friend class Randint;
  friend class RandintNode;
  friend class RealPart;
  friend class RightwardsArrowExpressionNode;
  friend class Round;
  friend class Secant;
  friend class Sequence;
  friend class SequenceNode;
  friend class SignFunction;
  friend class SimplificationHelper;
  friend class Sine;
  friend class SquareRoot;
  friend class SquareRootNode;
  friend class OStore;
  friend class StoreNode;
  friend class Subtraction;
  friend class SubtractionNode;
  friend class Sum;
  friend class SumAndProduct;
  friend class SumAndProductNode;
  friend class Symbol;
  friend class SymbolNode;
  friend class SymbolAbstract;
  friend class SymbolAbstractNode;
  friend class Tangent;
  friend class Trigonometry;
  friend class TrigonometryCheatTable;
  friend class OUnit;
  friend class UnitConvert;
  friend class UnitNode;
  friend class VectorCross;
  friend class VectorDot;
  friend class VectorNorm;

 public:
  static bool IsExpression() { return true; }

  /* Constructor & Destructor */
  OExpression() : PoolHandle() {}
  OExpression clone() const;
  static OExpression Parse(char const* string, Context* context,
                           bool addMissingParenthesis = true,
                           bool parseForAssignment = false);
  static OExpression ExpressionFromAddress(const void* address, size_t size);

  /* Hierarchy */
  OExpression childAtIndex(int i) const;
  void setChildrenInPlace(OExpression other) {
    node()->setChildrenInPlace(other);
  }

  /* Properties */
  ExpressionNode::Type otype() const { return node()->otype(); }
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const {
    return node()->isOfType(types);
  }
  TrinaryBoolean isPositive(Context* context) const {
    return node()->isPositive(context);
  }
  TrinaryBoolean isNull(Context* context) const {
    return node()->isNull(context);
  }
  bool isUndefined() const { return node()->isUndefined(); }
  bool allChildrenAreUndefined();
  bool allChildrenAreReal(Context* context,
                          bool canContainMatrices = true) const;
  bool isNumber() const { return node()->isNumber(); }
  bool isZero() const;
  bool isRationalOne() const;
  bool isOne() const;
  bool isMinusOne() const;
  bool isInteger() const;
  bool isRandomNumber() const { return node()->isRandomNumber(); }
  bool isRandomList() const { return node()->isRandomList(); }
  bool isRandom() const { return node()->isRandom(); }
  bool isParameteredExpression() const {
    return node()->isParameteredExpression();
  }
  bool isBasedIntegerCappedBy(const char* integerString) const;
  bool isDivisionOfIntegers() const;
  bool isAlternativeFormOfRationalNumber() const;
  template <typename T>
  bool hasDefinedComplexApproximation(
      const ApproximationContext& approximationContext,
      T* returnRealPart = nullptr, T* returnImagPart = nullptr) const;
  bool isScalarComplex(
      Preferences::CalculationPreferences calculationPreferences) const;
  bool isCombinationOfUnits() const { return node()->isCombinationOfUnits(); }
  /* This two functions only return true if the discontinuity is not asymptotic
   * (i.e. for the functions random, randint, round, floor and ceil).
   * Functions like 1/x are not handled here since it "obvious" that they are
   * discontinuous. */
  bool isSystemSymbol() const { return node()->isSystemSymbol(); }

  bool involvesDiscontinuousFunction(Context* context) const;
  bool isDiscontinuousBetweenValuesForSymbol(
      const char* symbol, float x1, float x2,
      const ApproximationContext& approximationContext) const;
  bool hasBooleanValue() const;
  bool hasMatrixOrListChild(Context* context, bool isReduced = true) const {
    return node()->hasMatrixOrListChild(context, isReduced);
  }

  /* recursivelyMatches will test each node recursively with the function
   * provided as argument. If the result is True, it will stop searching and
   * return true. If the result is Unknown, it will continue searching for a
   * True in the children. If an expression is tested as False, its children
   * will not be tested and the result will be false. */
  typedef TrinaryBoolean (*ExpressionTrinaryTest)(const OExpression e,
                                                  Context* context,
                                                  void* auxiliary);
  struct IgnoredSymbols {
    Symbol* head;
    void* tail;
  };
  bool recursivelyMatches(
      ExpressionTrinaryTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr,
      IgnoredSymbols* ignoredSymbols = nullptr) const;
  typedef bool (*ExpressionTest)(const OExpression e, Context* context);
  bool recursivelyMatches(
      ExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*SimpleExpressionTest)(const OExpression e);
  bool recursivelyMatches(
      SimpleExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*ExpressionTestAuxiliary)(const OExpression e, Context* context,
                                          void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const;

  bool deepIsOfType(std::initializer_list<ExpressionNode::Type> types,
                    Context* context = nullptr) const;
  bool deepIsMatrix(Context* context = nullptr, bool canContainMatrices = true,
                    bool isReduced = true) const;
  bool deepIsList(Context* context) const;
  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsUninitialized(const OExpression e) {
    return e.isUninitialized();
  }
  static bool IsUndefined(const OExpression e) { return e.isUndefined(); }
  static bool IsNAry(const OExpression e) {
    return e.isOfType(
        {ExpressionNode::Type::Addition, ExpressionNode::Type::Multiplication});
  }
  static bool IsApproximate(const OExpression e) {
    return e.isOfType({ExpressionNode::Type::Decimal,
                       ExpressionNode::Type::Float,
                       ExpressionNode::Type::Double});
  }
  static bool IsRandom(const OExpression e);
  static bool IsMatrix(const OExpression e, Context* context);
  static bool IsInfinity(const OExpression e) {
    return e.isOfType({ExpressionNode::Type::Infinity});
  }
  static bool IsPercent(const OExpression e) {
    return e.isOfType({ExpressionNode::Type::PercentSimple,
                       ExpressionNode::Type::PercentAddition});
  }
  static bool IsDiscontinuous(const OExpression e, Context* context);
  static bool IsSymbolic(const OExpression e);
  static bool IsPoint(const OExpression e) {
    return e.isUndefined() || e.otype() == ExpressionNode::Type::OPoint;
  }
  static bool IsSequence(const OExpression e) {
    return e.otype() == ExpressionNode::Type::Sequence;
  }
  static bool IsFactorial(const OExpression e) {
    return e.otype() == ExpressionNode::Type::Factorial;
  }

  typedef bool (*PatternTest)(const OExpression& e, Context* context,
                              const char* symbol);
  static bool IsRationalFraction(const OExpression& e, Context* context,
                                 const char* symbol);
  bool isLinearCombinationOfFunction(Context* context, PatternTest testFunction,
                                     const char* symbol) const;
  bool deepIsSymbolic(Context* context,
                      SymbolicComputation replaceSymbols) const;

  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynomial
   * - the degree of the polynomial otherwise */
  int polynomialDegree(Context* context, const char* symbolName) const {
    return this->node()->polynomialDegree(context, symbolName);
  }
  /* getVariables fills the matrix variables with the symbols in the expression
   * that pass the test isVariable. It returns the number of entries filled in
   * variables. For instance, getVariables of 'x+y+2*w/cos(4)' would result in
   * variables = {"x", "y", "w"} and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1.
   * If one of the variable lengths overflows maxVariableSize, getVariables
   * returns -2. */
  constexpr static int k_maxNumberOfVariables = 6;
  int getVariables(Context* context, ExpressionNode::isVariableTest isVariable,
                   char* variables, int maxVariableSize,
                   int nextVariableIndex = 0) const {
    return node()->getVariables(context, isVariable, variables, maxVariableSize,
                                nextVariableIndex);
  }
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char* variables, int maxVariableSize,
                             OExpression coefficients[], OExpression* constant,
                             Context* context,
                             Preferences::ComplexFormat complexFormat,
                             Preferences::AngleUnit angleUnit,
                             Preferences::UnitFormat unitFormat,
                             SymbolicComputation symbolicComputation) const;
  constexpr static int k_maxPolynomialDegree = 3;
  constexpr static int k_maxNumberOfPolynomialCoefficients =
      k_maxPolynomialDegree + 1;
  int getPolynomialReducedCoefficients(const char* symbolName,
                                       OExpression coefficients[],
                                       Context* context,
                                       Preferences::ComplexFormat complexFormat,
                                       Preferences::AngleUnit angleUnit,
                                       Preferences::UnitFormat unitFormat,
                                       SymbolicComputation symbolicComputation,
                                       bool keepDependencies = false) const;
  OExpression replaceSymbolWithExpression(const SymbolAbstract& symbol,
                                          const OExpression& expression) {
    return node()->replaceSymbolWithExpression(symbol, expression);
  }

  // These three functions are helpers for the function additional results
  int numberOfNumericalValues() const {
    return node()->numberOfNumericalValues();
  }
  /* Generalizes an expression to a function, by replacing numerical values with
   * a symbol. Beware that 2^3 is generalized as x^3, not x^x. */
  void replaceNumericalValuesWithSymbol(Symbol x);
  /* Return any numerical value in the expression. */
  float getNumericalValue() const;

  /* Units */
  // Call this method on properly reduced expressions only
  OExpression removeUnit(OExpression* unit) { return node()->removeUnit(unit); }
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false, Context* ctx = nullptr) const;
  bool isPureAngleUnit() const;
  bool isInRadians(Context* context) const;

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  bool hasComplexI(
      Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  // WARNING: this methods must be called on reduced expressions
  bool isReal(Context* context, bool canContainMatrices = true) const;

  static void SetReductionEncounteredUndistributedList(bool encounter);

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const OExpression e) const;
  /* isIdenticalToWithoutParentheses behaves as isIdenticalTo, but without
   * taking into account parentheses: e^(0) is identical to e^0. */
  bool isIdenticalToWithoutParentheses(const OExpression e) const;
  bool containsSameDependency(const OExpression e,
                              const ReductionContext& reductionContext) const;

  static bool ExactAndApproximateExpressionsAreEqual(
      OExpression exactExpression, OExpression approximateExpression);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context,
                      bool forceStripMargin = false, bool nested = false) const;
  ExpressionNode::LayoutShape leftLayoutShape() const {
    return node()->leftLayoutShape();
  }
  ExpressionNode::LayoutShape rightLayoutShape() const {
    return node()->rightLayoutShape();
  }

  /* TODO:
   * - Use same convention as strlcpy: return size of the source even if the
   * bufferSize was too small.*/
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits =
                       PrintFloat::k_maxNumberOfSignificantDigits) const;

  /* Simplification */
  /* Simplification routines are divided in 2 groups:
   * - ParseAndSimplify & simplify methods are used before approximating the
   *   expression. We simplify beforehand to avoid precision error but the
   *   simplified expression is never displayed. The ReductionTarget is
   *   therefore the System for these methods.
   * - ParseAndSimplifyAndApproximate & simplifyAndApproximate methods are used
   *   to simplify and approximate the expression for the User. They take into
   *   account the complex format required in the expression they return.
   *   (For instance, in Polar mode, they return an expression of the form
   *   r*e^(i*th) reduced and approximated.) */
  static OExpression ParseAndSimplify(
      const char* text, Context* context,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      UnitConversion unitConversion = UnitConversion::Default,
      bool* reductionFailure = nullptr);
  OExpression cloneAndSimplify(ReductionContext reductionContext,
                               bool* reductionFailure = nullptr) const;
  void cloneAndSimplifyAndApproximate(
      OExpression* simplifiedExpression, OExpression* approximateExpression,
      const ReductionContext& reductionContext,
      bool approximateKeepingSymbols = false) const;
  OExpression cloneAndReduce(ReductionContext reductionContext) const;
  /* TODO: deepReduceWithSystemCheckpoint should be private but we need to make
   * poincare/test/helper.h a class to be able to friend it */
  OExpression cloneAndDeepReduceWithSystemCheckpoint(
      ReductionContext* reductionContext, bool* reduceFailure,
      bool approximateDuringReduction = false) const;
  OExpression cloneAndReduceAndRemoveUnit(ReductionContext reductionContext,
                                          OExpression* unit) const;
  OExpression cloneAndReduceOrSimplify(ReductionContext reductionContext,
                                       bool beautify) {
    return beautify ? cloneAndSimplify(reductionContext)
                    : cloneAndReduce(reductionContext);
  }
  /* WARNING: this must be called only on expressions that:
   *  - are reduced.
   *  - have a known sign. (isPositive() != Unknown) */
  OExpression setSign(bool positive, const ReductionContext& reductionContext);

  OExpression deepRemoveUselessDependencies(
      const ReductionContext& reductionContext);
  OExpression deepReplaceSymbols(const ReductionContext& reductionContext);

  /* 'ExpressionWithoutSymbols' replaces symbols in place and returns an
   * uninitialized expression if it is circularly defined.
   * SymbolicComputation defines how to handle functions
   * and undefined symbols. */
  static OExpression ExpressionWithoutSymbols(
      OExpression expressionWithSymbols, Context* context,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);

  OExpression radianToAngleUnit(Preferences::AngleUnit angleUnit);
  OExpression angleUnitToRadian(Preferences::AngleUnit angleUnit);

  /* Returns:
   * - a number >= 0 if all the lists have the same size,
   * - -1 if there are no lists in the children
   * - -2 if there are lists of differents lengths. */
  constexpr static int k_noList = -1;
  constexpr static int k_mismatchedLists = -2;
  /* LengthOfListChildren is to be called during reduction, when all children
   * are already reduced. If you expression is NAry, it must be sorted before
   * calling this function. */
  int lengthOfListChildren() const;

  /* Approximation Helper
   * These methods reset the sApproximationEncounteredComplex flag. They should
   * not be use to implement node approximation */
  template <typename U>
  static U ParseAndSimplifyAndApproximateToScalar(
      const char* text, Context* context,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  template <typename U>
  OExpression approximate(
      const ApproximationContext& approximationContext) const;
  // WARNING: this will reduce the expression before removing units
  template <typename U>
  OExpression approximateKeepingUnits(
      const ReductionContext& reductionContext) const;
  template <typename U>
  U approximateToScalar(const ApproximationContext& approximationContext) const;
  template <typename U>
  Evaluation<U> approximateWithValueForSymbol(
      const char* symbol, U x,
      const ApproximationContext& approximationContext) const;
  template <typename U>
  U approximateToScalarWithValueForSymbol(
      const char* symbol, U x,
      const ApproximationContext& approximationContext) const;
  // This also reduces the expression. Approximation is in double.
  OExpression cloneAndApproximateKeepingSymbols(
      ReductionContext reductionContext) const;

  /* This class is meant to contain data about named functions (e.g. sin,
   * tan...) in one place: their name, their number of children and a pointer to
   * a builder. It is used in particular by the parser. */
  class FunctionHelper {
   public:
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             OExpression (*const builder)(OExpression))
        : FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren,
                         builder, nullptr, 0) {}
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int numberOfChildren,
                             OExpression (*const builder)(OExpression))
        : FunctionHelper(aliasesList, numberOfChildren, numberOfChildren,
                         builder) {}
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             PoolObject::Initializer initializer, size_t size)
        : FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren,
                         nullptr, initializer, size) {}
    constexpr AliasesList aliasesList() const { return m_aliasesList; }
    int minNumberOfChildren() const { return m_minNumberOfChildren; }
    int maxNumberOfChildren() const { return m_maxNumberOfChildren; }
    int numberOfChildren() const {
      assert(m_minNumberOfChildren == m_maxNumberOfChildren);
      return m_minNumberOfChildren;
    }
    OExpression build(OExpression children) const;

   private:
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             OExpression (*const builder)(OExpression),
                             PoolObject::Initializer initializer, size_t size)
        : m_aliasesList(aliasesList),
          m_minNumberOfChildren(minNumberOfChildren),
          m_maxNumberOfChildren(maxNumberOfChildren),
          m_untypedBuilder(builder),
          m_initializer(initializer),
          m_size(size) {
      assert(minNumberOfChildren >= 0 &&
             minNumberOfChildren <= maxNumberOfChildren);
    }
    AliasesList m_aliasesList;
    const int m_minNumberOfChildren;
    const int m_maxNumberOfChildren;
    OExpression (*const m_untypedBuilder)(OExpression children);
    PoolObject::Initializer m_initializer;
    const size_t m_size;
  };

  /* Tuple */
  typedef std::initializer_list<OExpression> Tuple;

  /* Iterator */
  friend Direct<OExpression, ExpressionNode>;
  Direct<OExpression, ExpressionNode> directChildren() const {
    return Direct<OExpression, ExpressionNode>(*this);
  }
  template <typename U>
  Evaluation<U> approximateToEvaluation(
      const ApproximationContext& approximationContext) const;

  Ion::Storage::Record::ErrorStatus storeWithNameAndExtension(
      const char* baseName, const char* extension) const;

  OExpression(const ExpressionNode* n) : PoolHandle(n) {}

 protected:
  template <typename U>
  static OExpression UntypedBuilderOneChild(OExpression children) {
    assert(children.otype() == ExpressionNode::Type::OList);
    return U::Builder(children.childAtIndex(0));
  }
  template <typename U>
  static OExpression UntypedBuilderTwoChildren(OExpression children) {
    assert(children.otype() == ExpressionNode::Type::OList);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1));
  }
  template <typename U>
  static OExpression UntypedBuilderThreeChildren(OExpression children) {
    assert(children.otype() == ExpressionNode::Type::OList);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1),
                      children.childAtIndex(2));
  }
  template <typename U>
  static OExpression UntypedBuilderFourChildren(OExpression children) {
    assert(children.otype() == ExpressionNode::Type::OList);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1),
                      children.childAtIndex(2), children.childAtIndex(3));
  }
  template <typename U>
  static OExpression UntypedBuilderMultipleChildren(OExpression children) {
    // Only with OExpression classes implementing addChildAtIndexInPlace
    assert(children.otype() == ExpressionNode::Type::OList);
    int childrenNumber = children.numberOfChildren();
    assert(childrenNumber > 0);
    U expression = U::Builder({children.childAtIndex(0)});
    for (int i = 1; i < childrenNumber; ++i) {
      expression.addChildAtIndexInPlace(children.childAtIndex(i), i, i);
    }
    return std::move(expression);
  }

  template <class T>
  T convert() const {
    /* This function allows to convert OExpression to derived Expressions.
     * The asserts ensure that the OExpression can only be casted to another
     * OExpression, but this does not prevent OExpression types mismatches (cast
     * Float to Symbol for instance).
     *
     * We could have overridden the operator T(). However, even with the
     * 'explicit' keyword (which prevents implicit casts), direct initialization
     * are enable which can lead to weird code:
     * ie, you can write: 'Rational a(2); AbsoluteValue b(a);'
     * */

    assert(T::IsExpression());
    static_assert(sizeof(T) == sizeof(OExpression), "Size mismatch");
    return *reinterpret_cast<T*>(const_cast<OExpression*>(this));
  }

  static_assert(sizeof(PoolHandle::Tuple) == sizeof(Tuple), "Size mismatch");
  static const PoolHandle::Tuple& convert(const Tuple& l) {
    assert(sizeof(PoolHandle) == sizeof(OExpression));
    return reinterpret_cast<const PoolHandle::Tuple&>(l);
  }

  /* Reference */
  ExpressionNode* node() const {
    assert(identifier() != PoolObject::NoNodeIdentifier &&
           !PoolHandle::node()->isGhost());
    return static_cast<ExpressionNode*>(PoolHandle::node());
  }

  /* Hierarchy */
  OExpression parent() const;  // TODO try to inline
  OExpression replaceWithUndefinedInPlace();
  void defaultSetChildrenInPlace(OExpression other);
  void addChildAtIndexInPlace(PoolHandle t, int index,
                              int currentNumberOfChildren) = delete;
  void removeChildAtIndexInPlace(int i) = delete;
  void removeChildInPlace(PoolHandle t, int childNumberOfChildren) = delete;
  void removeChildrenInPlace(int currentNumberOfChildren) = delete;

  /* Properties */
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const {
    return node()->getPolynomialCoefficients(context, symbolName, coefficients);
  }
  OExpression deepReplaceSymbolWithExpression(const SymbolAbstract& symbol,
                                              const OExpression expression);
  /* 'deepReplaceReplaceableSymbols' replaces symbols in place. isCircular
   * indicates if it is circularly defined and has been interrupted.
   * SymbolicComputation defines how to handle functions and undefined symbols.
   */
  bool involvesCircularity(Context* context, int maxDepth,
                           const char** visitedSymbols,
                           int numberOfVisitedSymbols) {
    return node()->involvesCircularity(context, maxDepth, visitedSymbols,
                                       numberOfVisitedSymbols);
  }
  OExpression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
    return node()->deepReplaceReplaceableSymbols(
        context, isCircular, parameteredAncestorsCount, symbolicComputation);
  }
  OExpression defaultReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation);

  /* Simplification */
  static void SimplifyAndApproximateChildren(
      OExpression input, OExpression* simplifiedOutput,
      OExpression* approximateOutput, const ReductionContext& reductionContext);
  void beautifyAndApproximateScalar(OExpression* simplifiedExpression,
                                    OExpression* approximateExpression,
                                    ReductionContext userReductionContext);
  /* makePositiveAnyNegativeNumeralFactor looks for:
   * - a negative numeral
   * - a multiplication who has one numeral child whose is negative
   * and turns the negative factor into a positive one.
   * The given OExpression should already be reduced and the return OExpression
   * is reduced (only a numeral factor was potentially made positive, and if it
   *  was -1, it was removed from the multiplication).
   * Warning: this must be called on reduced expressions
   */
  OExpression shallowBeautify(const ReductionContext& reductionContext) {
    return node()->shallowBeautify(reductionContext);
  }
  OExpression makePositiveAnyNegativeNumeralFactor(
      const ReductionContext& reductionContext);
  OExpression denominator(const ReductionContext& reductionContext) const {
    return node()->denominator(reductionContext);
  }
  /* shallowReduce takes a copy of reductionContext and not a reference
   * because it might need to modify it during reduction, namely in
   * SimplificationHelper::undefinedOnMatrix */
  OExpression shallowReduce(ReductionContext reductionContext) {
    return node()->shallowReduce(reductionContext);
  }
  OExpression deepBeautify(const ReductionContext& reductionContext) {
    return node()->deepBeautify(reductionContext);
  }

  /* Derivation */
  /* This method is used for the reduction of Derivative expressions.
   * It returns whether the instance is differentiable, and differentiates it if
   * able. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  void derivateChildAtIndexInPlace(int index,
                                   const ReductionContext& reductionContext,
                                   Symbol symbol, OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext) {
    return node()->unaryFunctionDifferential(reductionContext);
  }

  constexpr static int k_maxSymbolReplacementsCount = 10;

 public:
  /* Add missing parenthesis will add parentheses that easen the reading of the
   * expression or that are required by math rules. For example:
   * 2+-1 --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  OExpression addMissingParentheses();
  void shallowAddMissingParenthesis();

 private:
  /* Simplification */
  /* The largest integer such that all smaller integers can be stored without
   * any precision loss in IEEE754 double representation is 2E53 as the
   * mantissa is stored on 53 bits (2E308 can be stored exactly in IEEE754
   * representation but some smaller integers can't - like 2E308-1). */
  constexpr static double k_largestExactIEEE754Integer = 9007199254740992.0;
  OExpression deepReduce(ReductionContext reductionContext);
  void deepReduceChildren(const ReductionContext& reductionContext) {
    node()->deepReduceChildren(reductionContext);
  }

  OExpression shallowReduceUsingApproximation(
      const ReductionContext& reductionContext);
  OExpression defaultShallowBeautify() { return *this; }

  OExpression defaultUnaryFunctionDifferential() { return *this; }

  /* Approximation */
  OExpression deepApproximateKeepingSymbols(ReductionContext reductionContext,
                                            bool* parentCanApproximate,
                                            bool* parentShouldReduce);
  OExpression deepApproximateKeepingSymbols(ReductionContext reductionContext) {
    bool dummy = false;
    return deepApproximateKeepingSymbols(reductionContext, &dummy, &dummy);
  }
  void deepApproximateChildrenKeepingSymbols(
      const ReductionContext& reductionContextbool, bool* canApproximate,
      bool* shouldReduce);

  /* Properties */
  int defaultGetPolynomialCoefficients(int degree, Context* context,
                                       const char* symbol,
                                       OExpression expression[]) const;

  /* Builder */
  static OExpression CreateComplexExpression(
      OExpression ra, OExpression tb, Preferences::ComplexFormat complexFormat,
      bool isNegativeRa, bool isNegativeTb);
};

// Helper to create the expression associated to a node
template <typename T, typename U, int Nmin, int Nmax,
          typename Parent = OExpression>
class ExpressionBuilder : public Parent {
 public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<OExpression, Parent>::value);
#endif
  ExpressionBuilder(const U* n) : Parent(n) {}
  static T Builder() {
    static_assert(Nmin <= 0 && Nmax >= 0);
    PoolHandle h =
        PoolHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(OExpression child) {
    static_assert(Nmin <= 1 && Nmax >= 1);
    PoolHandle h =
        PoolHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(OExpression child1, OExpression child2) {
    static_assert(Nmin <= 2 && Nmax >= 2);
    PoolHandle h = PoolHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(OExpression child1, OExpression child2, OExpression child3) {
    static_assert(Nmin <= 3 && Nmax >= 3);
    PoolHandle h = PoolHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2, child3});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(OExpression child1, OExpression child2, OExpression child3,
                   OExpression child4) {
    static_assert(Nmin <= 4 && Nmax >= 4);
    PoolHandle h = PoolHandle::BuilderWithChildren(
        Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    T expression = static_cast<T&>(h);
    return expression;
  }
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper(U::k_functionName, Nmin, Nmax, Initializer<U>,
                                  sizeof(U));
};

template <typename T, typename U, typename P = OExpression>
using ExpressionNoChildren = ExpressionBuilder<T, U, 0, 0, P>;
template <typename T, typename U, typename P = OExpression>
using ExpressionOneChild = ExpressionBuilder<T, U, 1, 1, P>;
template <typename T, typename U, typename P = OExpression>
using ExpressionTwoChildren = ExpressionBuilder<T, U, 2, 2, P>;
template <typename T, typename U, typename P = OExpression>
using ExpressionUpToTwoChildren = ExpressionBuilder<T, U, 1, 2, P>;
template <typename T, typename U, typename P = OExpression>
using ExpressionThreeChildren = ExpressionBuilder<T, U, 3, 3, P>;
template <typename T, typename U, typename P = OExpression>
using ExpressionFourChildren = ExpressionBuilder<T, U, 4, 4, P>;
}  // namespace Poincare

#endif
