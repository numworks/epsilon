#include "prefix.h"

#include <assert.h>
#include <omg/float.h>
#include <omg/round.h>
#include <omg/unreachable.h>

#include <cmath>

#include "helpers.h"

namespace Poincare::Internal {
namespace Units {

/* Prefixes defined below must be defined only once and all units must be
 * constructed from their pointers. This way we can easily check if two Prefix
 * objects are equal by comparing pointers. This saves us from overloading the
 * == operator on Prefix and saves time at execution. */
static constexpr Prefix k_prefixes[Prefix::k_numberOfPrefixes] = {
    Prefix("p", -12), Prefix("n", -9), Prefix("μ", -6), Prefix("m", -3),
    Prefix("c", -2),  Prefix("d", -1), Prefix("", 0),   Prefix("da", 1),
    Prefix("h", 2),   Prefix("k", 3),  Prefix("M", 6),  Prefix("G", 9),
    Prefix("T", 12),
};

static_assert(k_prefixes[Prefix::k_emptyPrefixIndex].exponent() == 0,
              "Index for the Empty Prefix is incorrect.");
static_assert(k_prefixes[Prefix::k_kiloPrefixIndex].exponent() == 3,
              "Index for the Kilo Prefix is incorrect.");

const Prefix* Prefix::Prefixes() { return k_prefixes; }

const Prefix* Prefix::EmptyPrefix() { return Prefixes() + k_emptyPrefixIndex; }

uint8_t Prefix::ToId(const Prefix* prefix) {
  uint8_t id = (prefix - Prefixes());
  assert(FromId(id) == prefix);
  return id;
}

const Prefix* Prefix::FromId(uint8_t id) {
  assert(id < k_numberOfPrefixes);
  return Prefixes() + id;
}

bool PrefixSetContainsPrefix(PrefixSet prefixSet, const Prefix* prefix) {
  using enum PrefixSet;

  if (prefix->exponent() == 0) {
    return true;
  }
  if (prefixSet == None) {
    return false;
  }
  if (prefixSet == All) {
    return true;
  }
  if (prefixSet == LongScale) {
    return prefix->exponent() % 3 == 0;
  }
  if (prefixSet == NegativeAndKilo) {
    return prefix->exponent() < 0 || prefix->exponent() == 3;
  }
  if (prefixSet == NegativeLongScale) {
    return prefix->exponent() < 0 && prefix->exponent() % 3 == 0;
  }
  if (prefixSet == PositiveLongScale) {
    return prefix->exponent() > 0 && prefix->exponent() % 3 == 0;
  }
  if (prefixSet == Negative) {
    return prefix->exponent() < 0;
  }
  if (prefixSet == Positive) {
    return prefix->exponent() > 0;
  }
  OMG_UNREACHABLE;
}

const Prefix* ChooseBestPrefix(PrefixSet prefixSet, double value,
                               double exponent) {
  if (prefixSet == PrefixSet::None) {
    return Prefix::EmptyPrefix();
  }
  if (value < OMG::Float::EpsilonLax<double>()) {
    return Prefix::EmptyPrefix();
  }
  const Prefix* res = Prefix::EmptyPrefix();
  const float magnitude = std::log10(std::fabs(value));
  float bestOrder = magnitude;
  for (int i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    if (!PrefixSetContainsPrefix(prefixSet, Prefix::Prefixes() + i)) {
      continue;
    }
    float order = magnitude - Prefix::Prefixes()[i].exponent() * exponent;
    if (CompareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      res = Prefix::Prefixes() + i;
    }
  }
  return res;
}

}  // namespace Units
}  // namespace Poincare::Internal
