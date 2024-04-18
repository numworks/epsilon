#ifndef POINCARE_EXPRESSION_UNIT_H
#define POINCARE_EXPRESSION_UNIT_H

#include <poincare/src/memory/tree.h>

#include <array>

#include "approximation.h"
#include "builtin.h"
#include "context.h"
#include "dimension_vector.h"
#include "k_tree.h"

namespace Poincare::Internal {
namespace Units {

/* The units having the same physical dimension are grouped together.
 * Each such group has a standard representative with a standard prefix.
 *
 * Each representative has
 *  - a root symbol
 *  - a definition, as the conversion ratio with the SI unit of the same
 *    dimensions
 *  - informations on how the representative should be prefixed.
 *
 * Given an representative and a Prefix allowed for that representative, one
 * may get a symbol and an expression tree. */

class Prefix {
  friend class Unit;

 public:
  constexpr static int k_numberOfPrefixes = 13;
  static const Prefix* Prefixes();
  static const Prefix* EmptyPrefix();
  // Assigning an id to each accessible prefixes
  static uint8_t ToId(const Prefix* representative);
  static const Prefix* FromId(uint8_t id);

  const char* symbol() const { return m_symbol; }
  int8_t exponent() const { return m_exponent; }
#if 0
  size_ serialize(char* buffer, size_t bufferSize,) const;
#endif

 private:
  constexpr Prefix(const char* symbol, int8_t exponent)
      : m_symbol(symbol), m_exponent(exponent) {}

  const char* m_symbol;
  int8_t m_exponent;
};

class Representative {
  friend class Unit;

 public:
  // Operators
  bool operator==(const Representative&) const = default;
  bool operator!=(const Representative&) const = default;
  enum class Prefixable {
    None,
    PositiveLongScale,
    NegativeLongScale,
    Positive,
    Negative,
    NegativeAndKilo,
    LongScale,
    All,
  };
  constexpr static int k_numberOfDimensions = 25;
  // Assigning an id to each accessible representatives
  static uint8_t ToId(const Representative* representative);
  static const Representative* FromId(uint8_t id);

  static const Representative* const* DefaultRepresentatives();
  static const Representative* RepresentativeForDimension(
      DimensionVector vector);

  virtual const DimensionVector dimensionVector() const = 0;
  virtual int numberOfRepresentatives() const = 0;
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const Representative* representativesOfSameDimension() const = 0;
  virtual bool isBaseUnit() const = 0;
  virtual const Representative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const Prefix** prefix) const {
    return defaultFindBestRepresentative(
        value, exponent, representativesOfSameDimension(),
        representativesOfSameDimension() + numberOfRepresentatives(), prefix);
  }
#if 0
    /* hasSpecialAdditionalExpressions return true if the unit has special
     * forms suchas as splits (for time and imperial units) or common
     * conversions (such as speed and energy). */
    virtual bool hasSpecialAdditionalExpressions(double value,
                                                 UnitFormat unitFormat) const {
      return false;
    }
    virtual int setAdditionalExpressions(
        double value, Expression* dest, int availableLength,
        const ReductionContext& reductionContext) const {
      return 0;
    }
#endif

  Aliases rootSymbols() const { return m_rootSymbols; }
  double ratio() const {
    return Approximation::To<double>(ratioExpressionReduced());
  }
  bool isInputPrefixable() const {
    return m_inputPrefixable != Prefixable::None;
  }
  bool isOutputPrefixable() const {
    return m_outputPrefixable != Prefixable::None;
  }
#if 0
  size_ serialize(char* buffer, size_t bufferSize,, const Prefix* prefix) const;
#endif
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const Representative** representative,
                               const Prefix** prefix) const;
  bool canParse(const char* symbol, size_t length, const Prefix** prefix) const;
  bool canPrefix(const Prefix* prefix, bool input) const;
  const Prefix* findBestPrefix(double value, double exponent) const;
  const Tree* ratioExpressionReduced() const {
    return Tree::FromBlocks(m_ratioExpression);
  }

 protected:
  // TODO it may be marked consteval with Clang but not with GCC
  template <KTreeConcept T>
  constexpr Representative(Aliases rootSymbol, T ratioExpression,
                           Prefixable inputPrefixable,
                           Prefixable outputPrefixable)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(static_cast<const Block*>(KTree(ratioExpression))),
        m_inputPrefixable(inputPrefixable),
        m_outputPrefixable(outputPrefixable) {}

  const Representative* defaultFindBestRepresentative(
      double value, double exponent, const Representative* begin,
      const Representative* end, const Prefix** prefix) const;

  Aliases m_rootSymbols;
  /* m_ratioExpression is the expression of the factor used to convert a unit
   * made of the representative and its base prefix into base SI units. ex :
   * m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m^3).
   * m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
   * */

  const Block* m_ratioExpression;
  const Prefixable m_inputPrefixable;
  const Prefixable m_outputPrefixable;
};

#if 0
class UnitNode final : public ExpressionNode {
 public:
  constexpr static int k_numberOfBaseUnits = 8;

  // Expression Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return TrinaryBoolean::True;
  }
  TrinaryBoolean isNull(Context* context) const override {
    return TrinaryBoolean::False;
  }
  Expression removeUnit(Expression* unit) override;

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  size_ serialize(char* buffer, size_t bufferSize,,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  /* Approximation */
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;

};
#endif

class Unit {
 public:
  constexpr static int k_numberOfBaseUnits = 8;
  /* Prefixes and Representatives defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on Prefix and Representative and
   * saves time at execution. As such, their constructor are private and can
   * only be accessed by their friend class Unit. */
  constexpr static const Prefix k_prefixes[Prefix::k_numberOfPrefixes] = {
      Prefix("p", -12), Prefix("n", -9), Prefix("μ", -6), Prefix("m", -3),
      Prefix("c", -2),  Prefix("d", -1), Prefix("", 0),   Prefix("da", 1),
      Prefix("h", 2),   Prefix("k", 3),  Prefix("M", 6),  Prefix("G", 9),
      Prefix("T", 12),
  };

  /* Define access points to some prefixes. */
  constexpr static int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
                "Index for the Empty Prefix is incorrect.");
  constexpr static int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
                "Index for the Kilo Prefix is incorrect.");

  using Prefixable = Representative::Prefixable;

  static bool CanParse(UnicodeDecoder* name,
                       const Representative** representative,
                       const Prefix** prefix);
  static void ChooseBestRepresentativeAndPrefixForValue(Tree* units,
                                                        double* value,
                                                        UnitFormat unitFormat);
#if 0
  static bool ShouldDisplayAdditionalOutputs(double value, const Tree* unit,
                                             UnitFormat unitFormat);
  static int SetAdditionalExpressions(const Tree* units, double value,
                                      Tree* dest, int availableLength,
                                      const ReductionContext& reductionContext,
                                      const Tree exactOutput);
  static Tree* BuildSplit(double value, const Unit* units, int length,
                          const ReductionContext& reductionContext);
  static Tree* ConvertTemperatureUnits(
      Tree* e, Unit unit, const ReductionContext& reductionContext);
#endif

  static bool AllowImplicitAddition(
      const Representative* smallestRepresentative,
      const Representative* biggestRepresentative);

  static bool IsUnitOrPowerOfUnit(const Tree* expr);
  static bool ForceMarginLeftOfUnit(const Tree* expr);

#if 0
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression shallowBeautify();

  static bool IsBaseUnit(const Tree* unit) {
    return GetRepresentative(unit)->isBaseUnit() &&
           GetPrefix(unit) == Prefix::EmptyPrefix();
  }
#endif
  static void ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                                double exponent,
                                                UnitFormat unitFormat,
                                                bool optimizePrefix);

  static bool IsNonKelvinTemperature(const Representative* representative);
  // Remove units and Convert tree to Kelvin.
  static void RemoveTemperatureUnit(Tree* root);
  // Replace with SI ratio only.
  static void RemoveUnit(Tree* unit);
  // Push Unit
  static Tree* Push(const Representative* unitRepresentative,
                    const Prefix* unitPrefixx = Prefix::EmptyPrefix());
  static const Representative* GetRepresentative(const Tree* unit);
  static void SetRepresentative(Tree* unit,
                                const Representative* representative);
  static const Prefix* GetPrefix(const Tree* unit);
  static void SetPrefix(Tree* unit, const Prefix* prefix);
};

Tree* ChooseBestDerivedUnits(DimensionVector* unitsExponents);
bool IsCombinationOfUnits(const Tree* expr);

bool HasUnit(const Tree* expr);
bool IsPureAngleUnit(const Tree* expr);

}  // namespace Units
}  // namespace Poincare::Internal

#endif
