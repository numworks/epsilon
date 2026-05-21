#pragma once

#include <stdint.h>

namespace Poincare::Internal {
namespace Units {

class Prefix {
 public:
  constexpr static int k_numberOfPrefixes = 13;
  constexpr static int k_maxTextLen = 2;  // "da"

  constexpr static int k_emptyPrefixIndex = 6;
  constexpr static int k_kiloPrefixIndex = 9;

  constexpr Prefix() = default;
  constexpr Prefix(const char* symbol, int8_t exponent)
      : m_symbol(symbol), m_exponent(exponent) {}

  static const Prefix* Prefixes();
  static const Prefix* EmptyPrefix();
  // Assigning an id to each accessible prefixes
  static uint8_t ToId(const Prefix* representative);
  static const Prefix* FromId(uint8_t id);

  constexpr const char* symbol() const { return m_symbol; }
  constexpr int8_t exponent() const { return m_exponent; }

  const char* m_symbol = nullptr;
  int8_t m_exponent = 0;
};

// Describe what subsets of Prefix are allowed for a Representative
enum class PrefixSet {
  None,
  PositiveLongScale,
  NegativeLongScale,
  Positive,
  Negative,
  NegativeAndKilo,
  LongScale,
  All,
};

bool PrefixSetContainsPrefix(PrefixSet prefixSet, const Prefix* prefix);

// Choose the best prefix from a set for displaying a value with the given
// exponent
const Prefix* ChooseBestPrefix(PrefixSet prefixSet, double value,
                               double exponent);

}  // namespace Units
}  // namespace Poincare::Internal
