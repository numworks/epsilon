#ifndef POINCARE_EXPRESSION_UNIT_H
#define POINCARE_EXPRESSION_UNIT_H

#include <poincare_junior/src/memory/tree.h>

#include <array>

#include "approximation.h"
#include "builtin.h"
#include "context.h"
#include "k_tree.h"

namespace PoincareJ {
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
 * Given an representative and a UnitPrefix allowed for that representative, one
 * may get a symbol and an expression tree. */

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
  constexpr bool isSI() const {
    size_t numberOfOnes = 0;
    for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
      if (coefficientAtIndex(i) == 1) {
        numberOfOnes++;
      } else if (coefficientAtIndex(i) != 0) {
        return false;
      }
    }
    return numberOfOnes == 1;
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
  static const UnitRepresentative* RepresentativeForDimension(
      DimensionVector vector);

  virtual const DimensionVector dimensionVector() const = 0;
  virtual int numberOfRepresentatives() const = 0;
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const UnitRepresentative* representativesOfSameDimension() const = 0;
  virtual bool isBaseUnit() const = 0;
  virtual const UnitRepresentative* standardRepresentative(
      double value, double exponent, UnitFormat unitFormat,
      const UnitPrefix** prefix) const {
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
  int serialize(char* buffer, int bufferSize, const UnitPrefix* prefix) const;
#endif
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const UnitRepresentative** representative,
                               const UnitPrefix** prefix) const;
  bool canParse(const char* symbol, size_t length,
                const UnitPrefix** prefix) const;
  bool canPrefix(const UnitPrefix* prefix, bool input) const;
  const UnitPrefix* findBestPrefix(double value, double exponent) const;
  const Tree* ratioExpressionReduced() const {
    return Tree::FromBlocks(m_ratioExpression);
  }

 protected:
  // TODO it may be marked consteval with Clang but not with GCC
  template <TreeCompatibleConcept T>
  constexpr UnitRepresentative(Aliases rootSymbol, T ratioExpression,
                               Prefixable inputPrefixable,
                               Prefixable outputPrefixable)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(static_cast<const Block*>(KTree(ratioExpression))),
        m_inputPrefixable(inputPrefixable),
        m_outputPrefixable(outputPrefixable) {}

  const UnitRepresentative* defaultFindBestRepresentative(
      double value, double exponent, const UnitRepresentative* begin,
      const UnitRepresentative* end, const UnitPrefix** prefix) const;

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

  /* Define access points to some prefixes. */
  constexpr static int k_emptyPrefixIndex = 6;
  static_assert(k_prefixes[k_emptyPrefixIndex].m_exponent == 0,
                "Index for the Empty UnitPrefix is incorrect.");
  constexpr static int k_kiloPrefixIndex = 9;
  static_assert(k_prefixes[k_kiloPrefixIndex].m_exponent == 3,
                "Index for the Kilo UnitPrefix is incorrect.");

  using Prefixable = UnitRepresentative::Prefixable;

  static bool CanParse(UnicodeDecoder* name,
                       const UnitRepresentative** representative,
                       const UnitPrefix** prefix);
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
      const UnitRepresentative* smallestRepresentative,
      const UnitRepresentative* biggestRepresentative);

#if 0
  static bool ForceMarginLeftOfUnit(const Unit& unit);

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression shallowBeautify();

  static bool IsBaseUnit(const Tree* unit) {
    return GetRepresentative(unit)->isBaseUnit() &&
           GetPrefix(unit) == UnitPrefix::EmptyPrefix();
  }
#endif
  static void ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                                double exponent,
                                                UnitFormat unitFormat,
                                                bool optimizePrefix);

  // Replace with SI ratio only.
  static void RemoveUnit(Tree* unit);
  // Push Unit
  static Tree* Push(const UnitRepresentative* unitRepresentative,
                    const UnitPrefix* unitPrefixx = UnitPrefix::EmptyPrefix());
  static const UnitRepresentative* GetRepresentative(const Tree* unit);
  static void SetRepresentative(Tree* unit,
                                const UnitRepresentative* representative);
  static const UnitPrefix* GetPrefix(const Tree* unit);
  static void SetPrefix(Tree* unit, const UnitPrefix* prefix);
};

Tree* ChooseBestDerivedUnits(DimensionVector& unitsExponents);

}  // namespace Units
}  // namespace PoincareJ

#endif
