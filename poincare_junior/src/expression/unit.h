#ifndef POINCARE_EXPRESSION_UNIT_H
#define POINCARE_EXPRESSION_UNIT_H

#include <poincare_junior/src/memory/tree.h>

#include <array>

#include "approximation.h"
#include "builtin.h"
#include "context.h"
#include "k_tree.h"

namespace PoincareJ {

/* The units having the same physical dimension are grouped together.
 * Each such group has a standard representative with a standard prefix.
 *
 * Each representative has
 *  - a root symbol
 *  - a definition, as the conversion ratio with the SI unit of the same
 *    dimensions
 *  - informations on how the representative should be prefixed.
 *
 * Given an representative and a UnitPrefix allowed for that representative, one
 * may get a symbol and an expression tree. */

/* TODO: Rethink the representative structures to alleviate this header's size.
 * In unit.cpp:
 *  const EnergyRepresentatives energyRepresentatives = {
 *      .joules{
 *          "J",
 *          KTree(1.0_e).k_blocks,
 *          Prefixable::All,
 *          Prefixable::LongScale,
 *      },
 *      .electronVolts{
 *          "eV",
 *          KMult(1.602176634_e, KPow(10.0_e, -19_e)).k_blocks,
 *          Prefixable::All,
 *          Prefixable::LongScale,
 *      }};
 * In unit.h:
 *  struct EnergyRepresentatives {
 *    EnergyRepresentative joules;
 *    EnergyRepresentative electronVolts;
 *  };
 *  extern const EnergyRepresentatives energyRepresentatives;
 *  (...)
 *  constexpr static const EnergyRepresentative* k_energyRepresentatives =
 *        &energyRepresentatives.joules;
 */

// Copied from poincare/helpers.h
/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}

class UnitPrefix {
  friend class Unit;

 public:
  constexpr static int k_numberOfPrefixes = 13;
  static const UnitPrefix* Prefixes();
  static const UnitPrefix* EmptyPrefix();
  // Assigning an id to each accessible prefixes
  static uint8_t ToId(const UnitPrefix* representative);
  static const UnitPrefix* FromId(uint8_t id);

  const char* symbol() const { return m_symbol; }
  int8_t exponent() const { return m_exponent; }
#if 0
  int serialize(char* buffer, int bufferSize) const;
#endif

 private:
  constexpr UnitPrefix(const char* symbol, int8_t exponent)
      : m_symbol(symbol), m_exponent(exponent) {}

  const char* m_symbol;
  int8_t m_exponent;
};

struct DimensionVector {
  constexpr static uint8_t k_numberOfBaseUnits = 8;
  // Operators
  bool operator==(const DimensionVector&) const = default;
  bool operator!=(const DimensionVector&) const = default;
  // SupportSize is defined as the number of distinct base units.
  constexpr size_t supportSize() const {
    size_t supportSize = 0;
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      if (coefficientAtIndex(i) == 0) {
        continue;
      }
      supportSize++;
    }
    return supportSize;
  }
  constexpr bool isEmpty() const { return supportSize() == 0; }
  constexpr static DimensionVector Empty() { return {}; }

  static DimensionVector FromBaseUnits(const Tree* baseUnits);
  // Push SI units matching the vector
  Tree* toBaseUnits() const;

  void addAllCoefficients(const DimensionVector other, int8_t factor);
  void setCoefficientAtIndex(int8_t coefficient, uint8_t i);
  void setCoefficientAtIndex(int coefficient, uint8_t i) {
    assert(coefficient <= INT8_MAX && coefficient >= INT8_MIN);
    setCoefficientAtIndex(static_cast<int8_t>(coefficient), i);
  }
  constexpr int8_t coefficientAtIndex(uint8_t i) const {
    assert(i < k_numberOfBaseUnits);
    const int8_t coefficients[] = {time,
                                   distance,
                                   angle,
                                   mass,
                                   current,
                                   temperature,
                                   amountOfSubstance,
                                   luminousIntensity};
    static_assert(std::size(coefficients) == k_numberOfBaseUnits);
    return coefficients[i];
  }

  int8_t time = 0;
  int8_t distance = 0;
  int8_t angle = 0;
  int8_t mass = 0;
  int8_t current = 0;
  int8_t temperature = 0;
  int8_t amountOfSubstance = 0;
  int8_t luminousIntensity = 0;
};
static_assert(sizeof(DimensionVector) ==
              sizeof(uint8_t) * DimensionVector::k_numberOfBaseUnits);
static_assert(DimensionVector::Empty().isEmpty());

class UnitRepresentative {
  friend class Unit;

 public:
  // Operators
  bool operator==(const UnitRepresentative&) const = default;
  bool operator!=(const UnitRepresentative&) const = default;
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
  static uint8_t ToId(const UnitRepresentative* representative);
  static const UnitRepresentative* FromId(uint8_t id);

  static const UnitRepresentative* const* DefaultRepresentatives();
#if 0
  static const UnitRepresentative* RepresentativeForDimension(
      DimensionVector vector);
#endif
  constexpr UnitRepresentative(Aliases rootSymbol, const Block* ratioExpression,
                               Prefixable inputPrefixable,
                               Prefixable outputPrefixable)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(ratioExpression),
        m_inputPrefixable(inputPrefixable),
        m_outputPrefixable(outputPrefixable) {}

  virtual const DimensionVector dimensionVector() const {
    return DimensionVector::Empty();
  };
  virtual int numberOfRepresentatives() const { return 0; };
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const UnitRepresentative* representativesOfSameDimension() const {
    return nullptr;
  };
  virtual const UnitPrefix* basePrefix() const {
    return UnitPrefix::EmptyPrefix();
  }
  virtual bool isBaseUnit() const { return false; }
#if 0
  virtual const UnitRepresentative* standardRepresentative(
      double value, double exponent, const ReductionContext& reductionContext,
      const UnitPrefix** prefix) const {
    return defaultFindBestRepresentative(value, exponent,
                                         representativesOfSameDimension(),
                                         numberOfRepresentatives(), prefix);
  }
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
#if 0
  double ratio() const {
    return Approximation::To<double>(ratioExpressionReduced());
  }
#endif
  bool isInputPrefixable() const {
    return m_inputPrefixable != Prefixable::None;
  }
  bool isOutputPrefixable() const {
    return m_outputPrefixable != Prefixable::None;
  }
#if 0
  int serialize(char* buffer, int bufferSize, const UnitPrefix* prefix) const;
#endif
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const UnitRepresentative** representative,
                               const UnitPrefix** prefix) const;
  bool canParse(const char* symbol, size_t length,
                const UnitPrefix** prefix) const;
  bool canPrefix(const UnitPrefix* prefix, bool input) const;
#if 0
  const UnitPrefix* findBestPrefix(double value, double exponent) const;
#endif
  const Tree* ratioExpressionReduced() const {
    return Tree::FromBlocks(m_ratioExpression);
  }

 protected:
#if 0
  const UnitRepresentative* defaultFindBestRepresentative(
      double value, double exponent, const UnitRepresentative* representatives,
      int length, const UnitPrefix** prefix) const;
#endif

  Aliases m_rootSymbols;
  /* m_ratioExpression is the expression of the factor used to convert a unit
   * made of the representative and its base prefix into base SI units. ex :
   * m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m^3).
   * m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
   *
   * TODO: We have to use Block * instead of Tree * so that representatives
   * lists can be constexpr because casting Block* into Tree* requires a
   * reinterpret cast. */

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
  int serialize(char* buffer, int bufferSize,
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
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::OneLetter;
  }  // TODO

};
#endif

class Unit {
 public:
  constexpr static int k_numberOfBaseUnits = 8;
  /* Prefixes and Representatives defined below must be defined only once and
   * all units must be constructed from their pointers. This way we can easily
   * check if two Unit objects are equal by comparing pointers. This saves us
   * from overloading the == operator on UnitPrefix and UnitRepresentative and
   * saves time at execution. As such, their constructor are private and can
   * only be accessed by their friend class Unit. */
  constexpr static const UnitPrefix k_prefixes[UnitPrefix::k_numberOfPrefixes] =
      {
          UnitPrefix("p", -12), UnitPrefix("n", -9), UnitPrefix("μ", -6),
          UnitPrefix("m", -3),  UnitPrefix("c", -2), UnitPrefix("d", -1),
          UnitPrefix("", 0),    UnitPrefix("da", 1), UnitPrefix("h", 2),
          UnitPrefix("k", 3),   UnitPrefix("M", 6),  UnitPrefix("G", 9),
          UnitPrefix("T", 12),
  };
  using Prefixable = UnitRepresentative::Prefixable;

  static bool CanParse(UnicodeDecoder* name,
                       const UnitRepresentative** representative,
                       const UnitPrefix** prefix);
#if 0
  static void ChooseBestRepresentativeAndPrefixForValue(
      const Tree* units, double* value,
      const ReductionContext& reductionContext);
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

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_timeRepresentativesAllowingImplicitAddition[] = {
          &k_timeRepresentatives[0],  // s
          &k_timeRepresentatives[1],  // min
          &k_timeRepresentatives[2],  // h
          &k_timeRepresentatives[3],  // day
          &k_timeRepresentatives[5],  // month
          &k_timeRepresentatives[6],  // year
  };
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[0]
                                    ->m_rootSymbols,
                                "s"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[1]
                                    ->m_rootSymbols,
                                "min"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[2]
                                    ->m_rootSymbols,
                                "h"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[3]
                                    ->m_rootSymbols,
                                "day"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[4]
                                    ->m_rootSymbols,
                                "month"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_timeRepresentativesAllowingImplicitAddition[5]
                                    ->m_rootSymbols,
                                "year"),
                "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_distanceRepresentativesAllowingImplicitAddition[] = {
          &k_distanceRepresentatives[4],  // in
          &k_distanceRepresentatives[5],  // ft
          &k_distanceRepresentatives[6],  // yd
          &k_distanceRepresentatives[7]   // mi
  };
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "in"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "ft"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "yd"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_distanceRepresentativesAllowingImplicitAddition[3]->m_rootSymbols,
          "mi"),
      "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_massRepresentativesAllowingImplicitAddition[] = {
          &k_massRepresentatives[3],  // oz
          &k_massRepresentatives[4]   // lb
  };
  static_assert(StringsAreEqual(k_massRepresentativesAllowingImplicitAddition[0]
                                    ->m_rootSymbols,
                                "oz"),
                "Implicit addition between units has wrong unit");
  static_assert(StringsAreEqual(k_massRepresentativesAllowingImplicitAddition[1]
                                    ->m_rootSymbols,
                                "lb"),
                "Implicit addition between units has wrong unit");

  // These must be sorted in order, from smallest to biggest
  constexpr static const UnitRepresentative*
      k_angleRepresentativesAllowingImplicitAddition[] = {
          &k_angleRepresentatives[1],  // "
          &k_angleRepresentatives[2],  // '
          &k_angleRepresentatives[3]   // °
  };
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[0]->m_rootSymbols,
          "\""),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[1]->m_rootSymbols,
          "'"),
      "Implicit addition between units has wrong unit");
  static_assert(
      StringsAreEqual(
          k_angleRepresentativesAllowingImplicitAddition[2]->m_rootSymbols,
          "°"),
      "Implicit addition between units has wrong unit");

  struct RepresentativesList {
    const UnitRepresentative* const* representativesList;
    int length;
  };
  constexpr static RepresentativesList
      k_representativesAllowingImplicitAddition[] = {
          {k_timeRepresentativesAllowingImplicitAddition,
           std::size(k_timeRepresentativesAllowingImplicitAddition)},
          {k_distanceRepresentativesAllowingImplicitAddition,
           std::size(k_distanceRepresentativesAllowingImplicitAddition)},
          {k_massRepresentativesAllowingImplicitAddition,
           std::size(k_massRepresentativesAllowingImplicitAddition)},
          {k_angleRepresentativesAllowingImplicitAddition,
           std::size(k_angleRepresentativesAllowingImplicitAddition)}};
  constexpr static int k_representativesAllowingImplicitAdditionLength =
      std::size(k_representativesAllowingImplicitAddition);
  static bool AllowImplicitAddition(
      const UnitRepresentative* smallestRepresentative,
      const UnitRepresentative* biggestRepresentative);

  constexpr static const UnitRepresentative*
      k_representativesWithoutLeftMargin[] = {
          &k_angleRepresentatives[1],        // "
          &k_angleRepresentatives[2],        // '
          &k_angleRepresentatives[3],        // °
          &k_temperatureRepresentatives[1],  // °C
          &k_temperatureRepresentatives[2]   // °F
  };
  constexpr static int k_numberOfRepresentativesWithoutLeftMargin =
      std::size(k_representativesWithoutLeftMargin);
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[0]->m_rootSymbols, "\""),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[1]->m_rootSymbols, "'"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[2]->m_rootSymbols, "°"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[3]->m_rootSymbols, "°C"),
                "Wrong unit without margin");
  static_assert(StringsAreEqual(
                    k_representativesWithoutLeftMargin[4]->m_rootSymbols, "°F"),
                "Wrong unit without margin");

#if 0
  static bool ForceMarginLeftOfUnit(const Unit& unit);

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression shallowBeautify();

  static bool IsBaseUnit(const Tree* unit) {
    return GetRepresentative(unit)->isBaseUnit() &&
           GetPrefix(unit) == GetRepresentative(unit)->basePrefix();
  }
  void chooseBestRepresentativeAndPrefix(
      double* value, double exponent, const ReductionContext& reductionContext,
      bool optimizePrefix);
#endif

  // Replace with SI ratio only.
  static void RemoveUnit(Tree* unit);
  // Push Unit
  static Tree* Push(const UnitRepresentative* unitRepresentative,
                    const UnitPrefix* unitPrefix);
  // UnitRepresentative getter
  static const UnitRepresentative* GetRepresentative(const Tree* unit);
  // UnitPrefix getter
  static const UnitPrefix* GetPrefix(const Tree* unit);
};

}  // namespace PoincareJ

#endif
