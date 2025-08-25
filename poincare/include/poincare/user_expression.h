#pragma once

#include <poincare/context.h>
#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>
#include <poincare/helpers/parser.h>
#include <poincare/k_tree.h>
#include <poincare/math_options.h>
#include <poincare/point_or_scalar.h>
#include <poincare/pool_handle.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>

#include <span>

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
// TODO: Expose ProjectionContext
struct ProjectionContext;
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
  SystemExpression approximateUserToTree(AngleUnit angleUnit,
                                         ComplexFormat complexFormat,
                                         const Context& context) const;
  // Approximate real scalar or unit
  template <typename T>
  T approximateToRealScalar(AngleUnit angleUnit = AngleUnit::None,
                            ComplexFormat complexFormat = ComplexFormat::None,
                            const Context& context = EmptyContext{}) const;

  // Return NAN for all non real scalar expressions
  template <typename T>
  static T ParseAndSimplifyAndApproximateToRealScalar(
      const char* text, const Context& context, ComplexFormat complexFormat,
      AngleUnit angleUnit,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllSymbols);

  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      const Context& context = EmptyContext{},
                      OMG::Base base = OMG::Base::Decimal,
                      bool linearMode = false) const;
  char* toLatex(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits, const Context& context,
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
                                      const Context& context,
                                      T* returnRealPart = nullptr,
                                      T* returnImagPart = nullptr) const;
  bool isComplexScalar(
      Preferences::CalculationPreferences calculationPreferences,
      const Context& context) const;
  bool isParsedNumber() const;
  bool hasUnit(bool ignoreAngleUnits = false, bool* hasAngleUnits = nullptr,
               bool replaceSymbols = false,
               const Context& ctx = EmptyContext{}) const;

  /* TODO: The following two methods should be made private or deleted to hide
   *       Internal node types. */
  bool isOfType(std::initializer_list<Internal::AnyType> types) const;
  bool deepIsOfType(std::initializer_list<Internal::AnyType> types,
                    const Context& context = EmptyContext{}) const;

  /* recursivelyMatches */

  typedef OMG::Troolean (*UserExpressionTrinaryTest)(const UserExpression e,
                                                     const Context& context,
                                                     void* auxiliary);
  bool recursivelyMatches(UserExpressionTrinaryTest test,
                          const Context& context = EmptyContext{},
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols,
                          void* auxiliary = nullptr) const;

  typedef bool (*ExpressionTest)(const UserExpression e,
                                 const Context& context);
  bool recursivelyMatches(ExpressionTest test,
                          const Context& context = EmptyContext{},
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*SimpleExpressionTest)(const UserExpression e);
  bool recursivelyMatches(SimpleExpressionTest test,
                          const Context& context = EmptyContext{},
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (UserExpression::*NonStaticSimpleExpressionTest)() const;
  bool recursivelyMatches(NonStaticSimpleExpressionTest test,
                          const Context& context = EmptyContext{},
                          SymbolicComputation replaceSymbols =
                              SymbolicComputation::ReplaceDefinedSymbols) const;

  typedef bool (*ExpressionTestAuxiliary)(const UserExpression e,
                                          const Context& context,
                                          void* auxiliary);
  bool recursivelyMatches(ExpressionTestAuxiliary test,
                          const Context& context = EmptyContext{},
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
                          const Context& context,
                          SymbolicComputation replaceSymbols, void* auxiliary,
                          IgnoredSymbols* ignoredSymbols) const;

  UserExpression privateCloneAndSimplify(
      const Internal::ProjectionContext& context,
      bool* reductionFailure = nullptr) const;
};

}  // namespace Poincare
