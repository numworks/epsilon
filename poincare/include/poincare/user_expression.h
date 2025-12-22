#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>
#include <poincare/helpers/parser.h>
#include <poincare/k_tree.h>
#include <poincare/math_options.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/projection_context.h>
#include <poincare/symbol_context.h>

#include <span>

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class Layout;
class SystemExpression;

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
  // Build a List containing all the expression contained in [expressions]
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

  // TODO: Rework replaceSymbols as a const cloneAnd* methods.
  void replaceSymbols(const Poincare::SymbolContext& symbolContext,
                      SymbolicComputation symbolic =
                          SymbolicComputation::ReplaceDefinedSymbols);

  ComplexFormat preferedComplexFormat(
      ComplexFormat complexFormat, const SymbolContext& symbolContext,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols) const;

  bool updateProjectionContextWithPreferedComplexFormat(
      ProjectionContext& projContext) const;

  /* UserExpression to UserExpression helpers */

  /* Expressions in parameters are outputs. The return boolean indicates the
   * reduction status (success or failure) */
  bool cloneAndSimplifyAndApproximate(UserExpression* simplifiedExpression,
                                      UserExpression* approximatedExpression,
                                      ProjectionContext& context) const;
  UserExpression cloneAndSimplify(const ProjectionContext& context,
                                  bool* reductionFailure) const;
  /* This version does not warn if simplification fails. In case of failure the
   * initial expression is returned. */
  UserExpression cloneAndTrySimplify(const ProjectionContext& context) const;
  template <typename T>
  UserExpression cloneAndApproximate(ProjectionContext& context) const;

  // If of the form y-f(x), return f(x). Return uninitialized otherwise.
  UserExpression equivalentCartesianEquation() const;

  /* Other helpers */

  // Returns KFailedSimplification if the reduction failed
  SystemExpression cloneAndReduce(
      const ProjectionContext& projectionContext) const;
  template <typename T>
  SystemExpression approximateUserToTree(
      AngleUnit angleUnit, ComplexFormat complexFormat,
      const SymbolContext& symbolContext) const;
  // Approximate real scalar (returns NAN for non real scalars)
  template <typename T>
  T approximateToRealScalar(
      AngleUnit angleUnit = AngleUnit::None,
      ComplexFormat complexFormat = ComplexFormat::None,
      const SymbolContext& symbolContext = EmptySymbolContext{}) const;
  // Approximate scalar part of unit expression (returns NAN for non units)
  /* TODO_PCJ: This method is almost never used, and we might prefer to split it
   * into "removeUnit" and "approximateToRealScalar" methods */
  template <typename T>
  T approximateUnitToRealScalar(
      AngleUnit angleUnit = AngleUnit::None,
      ComplexFormat complexFormat = ComplexFormat::None,
      const SymbolContext& symbolContext = EmptySymbolContext{}) const;

  // Return NAN for all non real scalar expressions
  template <typename T>
  static T ParseAndSimplifyAndApproximateToRealScalar(
      const char* text, const SymbolContext& symbolContext,
      ComplexFormat complexFormat, AngleUnit angleUnit,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbols);

  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      const SymbolContext& symbolContext = EmptySymbolContext{},
                      OMG::Base base = OMG::Base::Decimal,
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

  /* recursivelyMatches */

  typedef OMG::Troolean (*UserExpressionTrinaryTest)(
      const UserExpression e, const SymbolContext& symbolContext,
      void* auxiliary);
  bool recursivelyMatches(
      UserExpressionTrinaryTest test,
      const SymbolContext& symbolContext = EmptySymbolContext{},
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols,
      void* auxiliary = nullptr) const;

  typedef bool (*ExpressionTest)(const UserExpression e,
                                 const SymbolContext& symbolContext);
  bool recursivelyMatches(
      ExpressionTest test,
      const SymbolContext& symbolContext = EmptySymbolContext{},
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*SimpleExpressionTest)(const UserExpression e);
  bool recursivelyMatches(
      SimpleExpressionTest test,
      const SymbolContext& symbolContext = EmptySymbolContext{},
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (UserExpression::*NonStaticSimpleExpressionTest)() const;
  bool recursivelyMatches(
      NonStaticSimpleExpressionTest test,
      const SymbolContext& symbolContext = EmptySymbolContext{},
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*ExpressionTestAuxiliary)(const UserExpression e,
                                          const SymbolContext& symbolContext,
                                          void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test,
      const SymbolContext& symbolContext = EmptySymbolContext{},
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
  bool recursivelyMatches(UserExpressionTrinaryTest test,
                          const SymbolContext& symbolContext,
                          SymbolicComputation replaceSymbols, void* auxiliary,
                          IgnoredSymbols* ignoredSymbols) const;

  UserExpression privateCloneAndSimplify(
      const ProjectionContext& context, bool* reductionFailure = nullptr) const;

  template <typename T>
  T privateApproximateToRealScalar(
      AngleUnit angleUnit = AngleUnit::None,
      ComplexFormat complexFormat = ComplexFormat::None,
      const SymbolContext& symbolContext = EmptySymbolContext{}) const;
};

}  // namespace Poincare
