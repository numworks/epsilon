#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>
#include <poincare/float_precision.h>
#include <poincare/helpers/parser.h>
#include <poincare/k_tree.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/simplification_parameters.h>
#include <poincare/simplification_policies.h>
#include <poincare/symbol_context.h>

#include <span>

#include "approximation_policy.h"

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class Layout;
class SystemExpression;

/* Used by deepSearch. These struct must be defined outside of UserExpression
 * for their default constructors to properly work. */
struct DeepSearchParameters {
  const SymbolContext& context = k_emptySymbolContext;
  SymbolicComputation symbolic = SymbolicComputation::ReplaceDefinedSymbols;
  void* auxiliary = nullptr;
};
struct SimpleDeepSearchParameters {
  const SymbolContext& context = k_emptySymbolContext;
  SymbolicComputation symbolic = SymbolicComputation::ReplaceDefinedSymbols;
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
  static UserExpression Builder(AngleUnit angleUnit);
  // Build a List containing all the expressions contained in [expressions]
  static UserExpression BuildListOfExpressions(
      std::span<UserExpression> expressions);
  // Build an expression of Undefined tree.
  static UserExpression Undefined();

  static UserExpression Parse(const Internal::Tree* layout,
                              const SymbolContext& symbolContext,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression Parse(const char* layout,
                              const SymbolContext& symbolContext,
                              ParserHelper::ParsingParameters params = {});
  static UserExpression ParseLatex(const char* latex,
                                   const SymbolContext& symbolContext,
                                   ParserHelper::ParsingParameters params = {});

  static UserExpression Create(const Internal::Tree* structure,
                               Internal::ContextTrees ctx);

  /* General helpers */

  UserExpression cloneChildAtIndex(int i) const;
  UserExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<UserExpression&>(clone);
  }

  UserExpression cloneAndReplaceSymbolWithExpression(
      const UserExpression& symbol, const Expression& expression,
      bool onlySecondTerm = false) const;
  UserExpression cloneAndReplaceSymbolWithUnknown(
      const UserExpression& symbol, bool onlySecondTerm = false) const;
  UserExpression cloneAndReplaceUnknownWithSymbol(CodePoint symbol) const;

  UserExpression cloneAndReplaceSymbols(
      const Poincare::SymbolContext& symbolContext,
      SymbolicComputation symbolic =
          SymbolicComputation::ReplaceDefinedSymbols) const;

  /* Change the user complex format from Real to Cartesian if the expression
   * contains i, Re, Im, Arg or Conj. */
  ComplexFormat preferredComplexFormat(
      ComplexFormat userComplexFormat, const SymbolContext& symbolContext,
      SymbolicComputation symbolicComputation) const;

  /* UserExpression to UserExpression helpers */

  /* Expressions in parameters are outputs. The return boolean indicates the
   * reduction status (success or failure)
   * The "optimize" flag should always be activated for approximation, as
   * simplification will check the dimension
   */
  bool cloneAndSimplifyAndApproximate(
      UserExpression* simplifiedExpression,
      UserExpression* approximatedExpression,
      const SimplificationParameters& simplificationParameters,
      const ApproximationPolicy& approximationPolicy = ApproximationPolicy{
          .optimize = true}) const;
  UserExpression cloneAndSimplify(
      const SimplificationParameters& simplificationParameters,
      bool* reductionFailure) const;
  /* This version does not warn if simplification fails. In case of failure the
   * initial expression is returned. */
  UserExpression cloneAndTrySimplify(
      const SimplificationParameters& simplificationParameters) const;
  UserExpression cloneAndApproximate(
      const UserParameters& userParameters,
      const ApproximationPolicy& approximationPolicy = ApproximationPolicy{},
      FloatPrecision floatPrecision = FloatPrecision::Double) const;

  // If of the form y-f(x), return f(x). Return uninitialized otherwise.
  UserExpression equivalentCartesianEquation() const;

  /* Other helpers */

  // Returns KFailedSimplification if the reduction failed
  SystemExpression cloneAndReduce(
      const SimplificationParameters& simplificationParameters) const;
  // Approximate real scalar (returns NAN for non real scalars)
  template <typename T>
  T approximateToRealScalar(const UserParameters& userParameters,
                            const ApproximationPolicy& approximationPolicy =
                                ApproximationPolicy{}) const;
  // Approximate scalar part of unit expression (returns NAN for non units)
  /* TODO_PCJ: This method is almost never used, and we might prefer to split it
   * into "removeUnit" and "approximateToRealScalar" methods */
  template <typename T>
  T approximateUnitToRealScalar(const UserParameters& userParameters,
                                const ApproximationPolicy& approximationPolicy =
                                    ApproximationPolicy{}) const;

  // Return NAN for all non real scalar expressions
  template <typename T>
  static T ParseAndSimplifyAndApproximateToRealScalar(
      const char* text,
      const SimplificationParameters& simplificationParameters);

  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      const SymbolContext& symbolContext = EmptySymbolContext{},
                      OMG::Base base = OMG::Base::Decimal,
                      bool useTenPowerInsteadOfE = false,
                      bool linearMode = false) const;
  char* toLatex(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits,
                const SymbolContext& symbolContext,
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
  bool hasDefinedComplexApproximation(AngleUnit angleUnit,
                                      ComplexFormat complexFormat,
                                      const SymbolContext& symbolContext,
                                      T* returnRealPart = nullptr,
                                      T* returnImagPart = nullptr) const;
  bool isComplexScalar(
      Preferences::CalculationPreferences calculationPreferences,
      const SymbolContext& symbolContext) const;
  bool isParsedNumber() const;
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false,
               const SymbolContext& symbolContext = EmptySymbolContext{}) const;

  //  --- Deep search ---
  /* TODO: Currently deep search must clone the expression as a UserExpression
   * to call the test on it, which is not efficient and useless since all the
   * calls to deepSearch are testing properties that can be tested directly on
   * the tree. If at some point trees are strongly typed, we should make these
   * tests take a Tree as parameter. */

  // Test types
  typedef OMG::Troolean (*UserExpressionTrinaryTest)(const UserExpression& e,
                                                     void* auxiliary);
  typedef bool (*UserExpressionBinaryTest)(const UserExpression& e,
                                           void* auxiliary);
  typedef bool (*UserExpressionSimpleTest)(const UserExpression& e);
  typedef bool (UserExpression::*NonStaticSimpleTest)() const;

  /**
   * Deep tests the expression and its children with the provided test.
   * - If the test returns `Troolean::True` or `Troolean::False`, the search
   * stops and the result is returned as a boolean.
   * - If it returns `Troolean::Unknown`, the search continues.
   * - Returns `false` at the end  if no test returned `True` or `False`.
   */
  bool deepSearch(UserExpressionTrinaryTest test,
                  const DeepSearchParameters& params = {}) const;

  /**
   * Deep tests the expression and its children with the provided test.
   * - If the test returns `true`, the search stops and returns `true`.
   * - If it returns `false`, the search continues.
   * - Returns `false` at the end if no test returned `true`.
   */
  bool deepSearch(UserExpressionBinaryTest test,
                  const DeepSearchParameters& params = {}) const;
  // See comment above other implementations for details
  bool deepSearch(UserExpressionSimpleTest test,
                  const SimpleDeepSearchParameters& params = {}) const;
  // See comment above other implementations for details
  bool deepSearch(NonStaticSimpleTest test,
                  const SimpleDeepSearchParameters& params = {}) const;

 private:
  UserExpression privateCloneAndSimplify(
      const SimplificationParameters& simplificationParameters,
      bool* reductionFailure = nullptr) const;

  template <typename T>
  T privateApproximateToRealScalar(
      const UserParameters& userParameters,
      const ApproximationPolicy& approximationPolicy =
          ApproximationPolicy{}) const;
};

}  // namespace Poincare
