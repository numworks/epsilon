#pragma once

#include <omg/signaling_nan.h>
#include <omg/variant.h>
#include <omg/vector.h>
#include <poincare/approximation_policy.h>
#include <poincare/float_precision.h>
#include <poincare/point_or_scalar.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/symbol_context.h>
#include <poincare/user_parameters.h>

#include <cmath>
#include <complex>

#include "random.h"

namespace Poincare::Internal {

struct Dimension;

/* This is the internal Approximation API, to be used when working inside
 * Poincare::Internal only.
 * Approximation can be called on all expression types (see See
 * ExpressionVariantParameters::Type). However depending on the expression type,
 * the available parameters and processing are slightly different. For example,
 * if the expression is User, UserParameters (angle unit, complex format, symbol
 * context) need to be passed. See the PrepareTree method which contains the
 * code that depends on the expression type. */

namespace Approximation {

// Expression type information of a Tree
enum class ExpressionType : uint8_t {
  User,
  System,
  PreparedFunction,
  Unspecified
};

/* The approximation of a System expression has an optional processing step
 * called preparation, that enhances the approximation precision.
 * User expressions cannot be prepared because they are not projected. */
struct SystemExpressionPolicy {
  bool prepare = false;
};

/* To indicate that an expression is already prepared for approximation (for
 * PreparedFunction types) */
struct Prepared {};

/*  To indicate that an expression does not relate to any Expression type (User,
 * System or PreparedFunction). In that case, no UserParameters can be passed
 * and the expression cannot be prepared. */
struct Unspecified {};

class ExpressionVariantParameters {
 public:
  ExpressionVariantParameters(const UserParameters& userParameters) {
    m_data.init<UserParameters>(userParameters);
  }

  ExpressionVariantParameters(SystemExpressionPolicy systemExpressionPolicy) {
    m_data.init<SystemExpressionPolicy>(systemExpressionPolicy);
  }

  ExpressionVariantParameters(Prepared) { m_data.init<Prepared>(); }
  ExpressionVariantParameters(Unspecified) { m_data.init<Unspecified>(); }

  ExpressionType expressionType() const {
    if (m_data.has<UserParameters>()) {
      return ExpressionType::User;
    } else if (m_data.has<SystemExpressionPolicy>()) {
      return ExpressionType::System;
    } else if (m_data.has<Prepared>()) {
      return ExpressionType::PreparedFunction;
    } else if (m_data.has<Unspecified>()) {
      return ExpressionType::Unspecified;
    }
    OMG_UNREACHABLE
  }

  template <typename T>
  T& get() {
    return m_data.get<T>();
  }
  template <typename T>
  const T& get() const {
    return m_data.get<T>();
  }

 private:
  OMG::Variant<UserParameters, SystemExpressionPolicy, Prepared, Unspecified>
      m_data;
};

struct Parameters {
  ApproximationPolicy policy = {};
  ExpressionVariantParameters expressionParameters = Unspecified{};
};

/* Approximation public methods */

// Main entrypoint
template <typename ReturnType>
ReturnType To(const Tree* e, FloatPrecision floatPrecision,
              const Parameters& parameters);

// If float precision can be deduced from the ReturnType
template <typename ReturnType>
ReturnType To(const Tree* e, const Parameters& parameters);

/* With an "abscissa" value for Variable<0>. Float precision is equal to
 * FloatType */
template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, const Parameters& parameters);

/* With an abscissa and a list element index */
template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, int16_t listElementIndex,
              const Parameters& parameters);

/* In-place approximation of a Tree*, returns true/false depending on whether
 * the expression has been modified. */
bool Approximate(Tree* e, FloatPrecision floatPrecision,
                 const Parameters& parameters);

/* Other public methods */

// Optimize a projected function for efficient approximations
void PrepareFunctionForApproximation(Tree* e, const char* variable,
                                     bool variableIsReal);

// Return false if e cannot be approximated to a defined value.
bool CanApproximate(const Tree* e, bool approxLocalVar = false);

constexpr int k_noActiveBranch = -1;
constexpr int k_unknownActiveBranch = -2;
/* Returns k_noActiveBranch if every condition is false, it assumes there is no
 * other free variable than VarX, return k_unknownActiveBranch if a condition
 * was undefined. */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x);

template <typename T>
T FloatBinomial(T n, T k);

Tree* ExtractRealPart(const Tree* e);

template <typename T>
bool IsNonReal(std::complex<T> x) {
  assert(!OMG::IsSignalingNan(x.real()) || x.imag() == static_cast<T>(0));
  return OMG::IsSignalingNan(x.real());
}

// Boolean return type for Approximation
class BooleanOrUndefined {
 public:
  // Boolean of given value
  constexpr BooleanOrUndefined(bool value) : m_value{value} {}
  // Undefined boolean
  constexpr static BooleanOrUndefined Undef() { return BooleanOrUndefined(); }

  bool isUndefined() const { return m_isUndefined; }
  bool value() const {
    assert(!m_isUndefined);
    return m_value;
  }

 private:
  constexpr BooleanOrUndefined() : m_isUndefined{true} {}
  bool m_isUndefined = false;
  /* In case of an undefined, the stored value is false (but it should not be
   * fetched) */
  bool m_value = false;
};

namespace Private {

/* Common approximation pipeline for all return types and context arguments
 * (abscissa, list element etc.) */
template <typename ReturnType, typename... ContextArguments>
ReturnType Process(const Tree* e, FloatPrecision floatPrecision,
                   const Parameters& parameters,
                   ContextArguments... contextArguments);

using VariableType = std::complex<double>;

/* LocalEvaluator stores the values of local variables. The variable that was
 * pushed last has index 0, then the second last has index 1, and so on.  */
class LocalEvaluator {
 public:
  // This parameter could be a project-dependent configuration
  static constexpr size_t k_maxNestedVariables = 8;

  constexpr LocalEvaluator() = default;
  explicit LocalEvaluator(VariableType x) { pushVariable(x); }

  bool isFull() const { return m_values.isFull(); }

  void pushVariable(VariableType value) {
    assert(!isFull());
    m_values.push(value);
  }

  void setLocalValue(VariableType value) { m_values.last() = value; }

  VariableType variable(uint8_t index) const {
    if (m_values.size() <= index) {
      return NAN;
    }
    return m_values[m_values.size() - 1 - index];
  }

  void leaveCurrentScope() { m_values.pop(); }

 private:
  OMG::StaticVector<VariableType, k_maxNestedVariables> m_values = {};
};

class Context {
 public:
  constexpr static int16_t k_unspecifiedListElement = -1;
  constexpr static int16_t k_unspecifiedPointElement = -1;

  Context() = default;
  explicit Context(VariableType abscissa)
      : localEvaluator{LocalEvaluator(abscissa)} {}
  Context(VariableType abscissa, int16_t listElementIndex)
      : localEvaluator{LocalEvaluator(abscissa)},
        listElement{listElementIndex} {}

  Random::Context randomContext{false};
  // LocalEvaluator is used to store the values of local variables
  LocalEvaluator localEvaluator{};
  // Tells if we are approximating to get the nth-element of a list
  int16_t listElement = k_unspecifiedListElement;
  // Tells if we are approximating to get the nth-element of a point
  int16_t pointElement = k_unspecifiedPointElement;
};

/* Helper methods */

/* If requested, project user variables or prepare the expression for
 * approximation */
Tree* PreprocessTree(const Tree* e,
                     const ExpressionVariantParameters& expressionParameters);

/* Undefined value representing an unhandled dimension for the T type (Tree,
 * float / double, boolean etc.)
 * TODO: this template method and its specializations can disappear once we find
 * a way to assert the dimension check in To instead of returning an
 * undefined value. */
template <typename T>
T UndefinedUnhandledDimension();

template <typename T>
std::complex<T> NonReal() {
  return std::complex<T>(OMG::SignalingNan<T>(), static_cast<T>(0));
}

enum class DefinedStatus { Defined, NonReal, Undefined };

/* Convert a DefinedStatus to an std::complex<T> value: NonReal<T>{} for NonReal
 * and std::complex<T>(NAN) for Undef. ToUndefinedComplexValue is not callable
 * on DefinedStatus::Defined. */
template <typename T>
std::complex<T> ToUndefinedComplexValue(DefinedStatus status);

// Expression must have been projected beforehand
bool PrepareExpressionForApproximation(Tree* e);
bool ShallowPrepareForApproximation(Tree* e, void* ctx);

bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision);
bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision,
                       TypeBlock parentType, int indexInParent,
                       bool previousChildWasApproximated);

/* Variables with id >= firstNonApproximableVarId are considered not
 * approximable. */
bool CanApproximate(const Tree* e, int firstNonApproximableVarId);

template <typename T>
std::complex<T> TrigonometricToComplex(TypeBlock type, std::complex<T> value,
                                       AngleUnit angleUnit);
template <typename T>
std::complex<T> HyperbolicToComplex(TypeBlock type, std::complex<T> value);

template <typename T>
std::complex<T> ComplexLogarithm(std::complex<T> c, bool isLog10);

template <typename T>
bool IsIntegerRepresentationAccurate(T x);

template <typename T>
T PositiveIntegerApproximation(T c);

template <typename T>
std::complex<T> NeglectRealOrImaginaryPartIfNegligible(
    std::complex<T> result, std::complex<T> input1,
    std::complex<T> input2 = 1.0, bool enableNullResult = true);

template <typename T>
std::complex<T> MakeResultRealIfInputIsReal(std::complex<T> result,
                                            std::complex<T> input);

// Integral helper classes

template <typename T>
struct DetailedResult {
  T integral;
  T absoluteError;
};

struct AlternativeIntegrand {
  double a;
  double b;
  const Tree* integrandNearA = nullptr;
  const Tree* integrandNearB = nullptr;
};

template <typename T>
struct Substitution {
  enum class Type { None, LeftOpen, RightOpen, RealLine };
  Type type;
  T originA;
  T originB;
};

class Approximator {
  /* Copy of the Poincare::UserParameters struct but the constness attributes
   * are a bit different (angleUnit must be mutable when approximating) */
  class InternalUserParameters {
   public:
    InternalUserParameters() = default;
    // Implicit conversion from external UserParameters
    InternalUserParameters(const UserParameters& userParameters);

    // Implicit conversion to external UserParameters
    operator UserParameters() const {
      return UserParameters {
#if POINCARE_COMPLEX
        .complexFormat = complexFormat,
#endif
        .angleUnit = angleUnit, .context = symbolContext
      };
    }

    static InternalUserParameters Make(
        const ExpressionVariantParameters& expressionParameters);

    const ComplexFormat complexFormat = ComplexFormat::None;
    // Mutable because of AngleUnitContext
    mutable AngleUnit angleUnit = AngleUnit::None;
    const Poincare::SymbolContext& symbolContext = k_emptySymbolContext;
  };

 public:
  Approximator(FloatPrecision floatPrecision,
               const ExpressionVariantParameters& expressionParameters,
               Private::Context&& context);

  /* Specific processing for the ReturnType. Must be overloaded for each
   * specific ReturnType */
  template <typename ReturnType>
  ReturnType specificApproximateTo(const Tree* e);

  /* Only makes sense for a Tree* ReturnType. The method raises an assertion for
   * other types.
   * Returns a clone of the input tree where every scalar subtree that can be
   * approximated has been approximated. */
  template <typename ReturnType>
  ReturnType approximateScalars(const Tree* e);

  /* Approximate the conditions of a piecewise and return the tree corresponding
   * to the matching branch. Return nullptr if a condition was undefined. */
  // TODO: move this method out of the Approximator class
  const Tree* selectPiecewiseBranch(const Tree* piecewise);

  // Replace a Tree with the Tree of its complex approximation
  inline void toComplexTreeInplace(Tree* e) {
    e->moveTreeOverTree(toComplexTree<double>(e));
  }

  FloatPrecision floatPrecision() const { return m_floatPrecision; }

 private:
  template <typename FloatType>
  std::complex<FloatType> toComplex(const Tree* e);

  // Tree must be of scalar dimension or equivalent. Escape if tree is too deep.
  template <typename T>
  std::complex<T> privateToComplex(const Tree* e);

  // Tree must be of scalar dimension or equivalent
  template <typename T>
  std::complex<T> privateToComplexRecursive(const Tree* e);

  template <typename T>
  std::complex<T> toComplexSwitch(const Tree* e);

  template <typename T>
  std::complex<T> basicToComplex(const Tree* e);

  template <typename T>
  std::complex<T> allTrigToComplex(const Tree* e);

  struct ParametricBoundOutput {
    int low;
    int up;
    bool isDefined;
  };

  template <typename T>
  ParametricBoundOutput getParametricBoundOutput(const Tree* e);

  template <typename T>
  std::complex<T> userSymbolToComplex(const Tree* e);

  template <typename T>
  std::complex<T> analysisToComplex(const Tree* e);

  template <typename T>
  std::complex<T> matrixToComplex(const Tree* e);

  template <typename T>
  std::complex<T> listToComplex(const Tree* e);

  template <typename T>
  std::complex<T> miscToComplex(const Tree* e);

  template <typename T>
  std::complex<T> toComplexSwitchOnlyReal(const Tree* e);

  template <typename T>
  std::complex<T> approximatePower(const Tree* e, ComplexFormat complexFormat);

  template <typename T>
  std::complex<T> approximateRoot(const Tree* e);

  template <typename T>
  std::complex<T> approximateTrace(const Tree* e);

  template <typename FloatType>
  FloatType toFloat(const Tree* e);

  // Tree must be of scalar dimension or equivalent, and real.
  template <typename T>
  T privateToFloat(const Tree* e);

  // Integral

  template <typename T>
  T approximateIntegral(const Tree* integral);

  /* Return a list of integrals, split at singularities. If no singularity is
   * detected, the list contains a single element, which is the clone of
   * "integral" */
  template <typename T>
  Tree* splitAtSingularities(const Tree* integral, T a, T b);

  // Integral approximation on an integral already split at singularities
  template <typename T>
  T approximateIntegralInternal(const Tree* integral);

  // Use a tanh-sinh quadrature
  template <typename T>
  T approximateIntegralSingularityAtBoundsMethod(const Tree* integral, T a, T b,
                                                 bool notFiniteInA,
                                                 bool notFiniteInB);

  // Use an adaptive Kronrod-Gauss quadrature
  template <typename T>
  T approximateIntegralGenericMethod(const Tree* integral, T a, T b);

  template <typename T>
  T evaluateIntegrand(const Tree* integrand, T x,
                      Private::Substitution<T> substitution);

  template <typename T>
  T evaluateIntegrandNearBound(
      const Tree* integrand, T x, T xc,
      Private::AlternativeIntegrand alternativeIntegrand);

  template <typename T>
  Private::DetailedResult<T> tanhSinhQuadrature(
      const Tree* integrand, int level,
      Private::AlternativeIntegrand alternativeIntegrand);

  template <typename T>
  Private::DetailedResult<T> kronrodGaussQuadrature(
      const Tree* integrand, T a, T b, Private::Substitution<T> substitution);

  template <typename T>
  Private::DetailedResult<T> adaptiveQuadrature(
      const Tree* integrand, T a, T b, Private::Substitution<T> substitution);

  template <typename T>
  Private::DetailedResult<T> iterateAdaptiveQuadrature(
      const Tree* integrand, Private::DetailedResult<T> quadKG, T a, T b,
      T absoluteErrorThreshold, T relativeErrorThreshold,
      Private::Substitution<T> substitution, int iterationDepth,
      int& iterationCount);

  // Derivative

  template <typename T>
  T approximateDerivative(const Tree* function, T at, int order);

  template <typename T>
  T riddersApproximation(int order, const Tree* child, T x, T h, T* error);

  template <typename T>
  T growthRateAroundAbscissa(T x, T h, int order, const Tree* child);

  // Random

  template <typename T>
  T approximateRandom(const Tree* random);

  double privateApproximateRandIntNoRep(const Tree* randIntNoRep, uint8_t seed);

  template <typename T>
  T approximateRandomHelper(const Tree* randomTree);

  // Tree can be of any dimension
  Tree* privateToTree(const Tree* e, Dimension dim);

  void approximateAndReplaceEveryScalar(Tree* e, bool isRoot = true);

  // Tree must be of point dimension.
  Tree* privateToPoint(const Tree* e);

  // Tree must be of matrix dimension.
  Tree* toMatrix(const Tree* e);

  template <typename T>
  Tree* evaluateMatrixPower(const Tree* e);

  // Tree must be of list dimension.
  Tree* toList(const Tree* e);

  /* Returns a global DefinedStatus for all the dependencies. The DefinedStatus
   * is prioritized: Defined if all dependencies are well defined, NonReal if
   * one or more dependencies are NonReal but none is Undefined, Undefined
   * otherwise. */
  template <typename T>
  Private::DefinedStatus areDependenciesDefined(const Tree* dep);

  template <typename T>
  Tree* toComplexTree(const Tree* e);

  // Tree must be of boolean dimension.
  BooleanOrUndefined privateToBoolean(const Tree* e);

  template <typename T>
  BooleanOrUndefined inequalityToBoolean(const Tree* e);

  template <typename T>
  BooleanOrUndefined comparisonToBoolean(const Tree* e);

  // Tree must be of point dimension.
  template <typename T>
  Coordinate2D<T> privateToCoordinates(const Tree* e);

  template <typename FloatType>
  PointOrRealScalar<FloatType> toPointOrRealScalar(const Tree* e);

  /* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
   * real. */
  template <typename T>
  T toLocalContext(const Tree* e, T x);

  const FloatPrecision m_floatPrecision;
  const InternalUserParameters m_parameters;
  Private::Context m_context;
};

}  // namespace Private

}  // namespace Approximation

}  // namespace Poincare::Internal
