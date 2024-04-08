#ifndef ION_EXAM_MODE_H
#define ION_EXAM_MODE_H

#include <ion/exam_bytes.h>
#include <kandinsky/color.h>
#include <omg/bit_helper.h>

namespace Ion {
namespace ExamMode {

using Int = ExamBytes::Int;

enum class Ruleset : Int {
  Off = 0,
  Standard,
  Dutch,
  IBTest,
  PressToTest,
  Portuguese,
  English,
  STAAR,
  Pennsylvania,
  SouthCarolina,
  NorthCarolina,
  NumberOfRulesets,
};

/* Encode exam mode permissions on a 16 bits integer.
 * The first bit "configurable" is set to 1 if "data" encodes press-to-test
 * flags. If it set to 0, "data" contains the index of the preset rules (Off,
 * Standard, Dutch...)
 * The bit "clearBit" is always 0 for valid configurations, to ensure
 * compatibility with the PersistingBytes API.
 */
class Configuration {
  enum class Bits : size_t {
    Configurable = 0,
    DataFirst,
    DataLast = 14,
    Cleared,
    NumberOfBits
  };

 public:
  constexpr static size_t k_dataSize = static_cast<size_t>(Bits::DataLast) -
                                       static_cast<size_t>(Bits::DataFirst) + 1;

  explicit Configuration(Ruleset rules, Int flags = 0);
  Configuration() : Configuration(-1) {}
  Configuration(Int raw) : m_bits(raw) {}

  bool operator==(const Configuration& other) const { return m_bits == m_bits; }
  bool operator!=(const Configuration& other) const {
    return !(*this == other);
  }

  Ruleset ruleset() const;
  Int flags() const;
  Int raw() const { return m_bits; }
  bool isUninitialized() const;
  bool isActive() const;
  KDColor color() const;

 private:
  static_assert(static_cast<int>(Bits::NumberOfBits) ==
                OMG::BitHelper::numberOfBitsIn<Int>());

  bool configurable() const { return m_bits.get(Bits::Configurable); }
  Int data() const { return m_bits.get(Bits::DataLast, Bits::DataFirst); }

  OMG::BitHelper::BitField<Int> m_bits;
};

static_assert(
    sizeof(Configuration) == sizeof(Int),
    "ExamMode::Configuration size not compatible with PersistingBytes");

Configuration get();
void set(Configuration);

}  // namespace ExamMode
}  // namespace Ion

#endif
