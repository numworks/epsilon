#pragma once

#include <poincare/simplification_parameters.h>
#include <poincare/src/memory/tree.h>

#include "../approximation.h"
#include "../builtin.h"
#include "../dimension_vector.h"
#include "prefix.h"

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
 * Given a representative and a Prefix allowed for that representative, one
 * may get a symbol and an expression tree. */

class Representative {
  friend class Unit;

 public:
  constexpr static int k_maxTextLen = 5;  // "month"

  // Operators
  bool operator==(const Representative&) const = default;
  bool operator!=(const Representative&) const = default;
#if POINCARE_UNIT_ONLY_ANGLES
  constexpr static int k_numberOfDimensions = 1;
#elif POINCARE_UNIT
  constexpr static int k_numberOfDimensions = 25;
#else
  constexpr static int k_numberOfDimensions = 0;
#endif
  // Assigning an id to each accessible representatives
  static uint8_t ToId(const Representative* representative);
  static const Representative* FromId(uint8_t id);

  static const Representative* const* DefaultRepresentatives();
  static const Representative* RepresentativeForDimension(SIVector vector);

  virtual const SIVector siVector() const = 0;
  virtual int numberOfRepresentatives() const = 0;
  /* representativesOfSameDimension returns a pointer to the array containing
   * all representatives for this's dimension. */
  virtual const Representative* representativesOfSameDimension() const = 0;
  virtual const Representative* bestRepresentativeAndPrefix(
      double value, double exponent, Preferences::UnitFormat unitFormat,
      const Prefix** prefix, const Representative* forcedRepr = nullptr) const {
    if (forcedRepr) {
      return defaultFindBestRepresentativeAndPrefix(value, exponent, forcedRepr,
                                                    forcedRepr + 1, prefix);
    }
    return defaultFindBestRepresentativeAndPrefix(
        value, exponent, representativesOfSameDimension(),
        representativesOfSameDimension() + numberOfRepresentatives(), prefix);
  }

  Aliases rootSymbols() const { return m_rootSymbols; }
  double ratio() const {
    return Approximation::To<double>(
        ratioExpression(),
        Approximation::Parameters{.policy = {.optimize = true}});
  }

  PrefixSet inputPrefixSet() const { return m_inputPrefixSet; }
  PrefixSet outputPrefixSet() const { return m_outputPrefixSet; }

  bool isImperial() const { return m_isImperial; }
  bool canParseWithEquivalents(const char* symbol, size_t length,
                               const Representative** representative,
                               const Prefix** prefix) const;
  bool canParse(const char* symbol, size_t length, const Prefix** prefix) const;
  const Tree* ratioExpression() const { return m_ratioExpression; }
  // Push reduced ratio expression on TreeStack
  Tree* pushReducedRatioExpression() const;

 protected:
  // TODO it may be marked consteval with Clang but not with GCC
  template <KTreeConcept T>
  constexpr Representative(Aliases rootSymbol, T ratioExpression,
                           PrefixSet inputPrefixSet, PrefixSet outputPrefixSet,
                           bool isImperial = false)
      : m_rootSymbols(rootSymbol),
        m_ratioExpression(ratioExpression),
        m_inputPrefixSet(inputPrefixSet),
        m_outputPrefixSet(outputPrefixSet),
        m_isImperial(isImperial) {
#if !PLATFORM_DEVICE
    /* This assert prohibits building device in debug because it generates code
     * in the .init_array section which is not handled */
    assert(!rootSymbol || strlen(rootSymbol.mainAlias()) <= k_maxTextLen);
#endif
  }

  const Representative* defaultFindBestRepresentativeAndPrefix(
      double value, double exponent, const Representative* begin,
      const Representative* end, const Prefix** prefix) const;

  Aliases m_rootSymbols;
  /* m_ratioExpression is the expression of the factor used to convert a unit
   * made of the representative and its base prefix into base SI units. ex :
   * m_ratio for Liter is 1e-3 (as 1_L = 1e-3_m^3).
   * m_ratio for gram is 1 (as k is its best prefix and _kg is SI)
   * */

  const Tree* m_ratioExpression;
  const PrefixSet m_inputPrefixSet;
  const PrefixSet m_outputPrefixSet;
  const bool m_isImperial;
};

}  // namespace Units
}  // namespace Poincare::Internal
