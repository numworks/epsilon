#ifndef ION_EXAM_MODE_H
#define ION_EXAM_MODE_H

#include <ion/exam_bytes.h>
#include <kandinsky/color.h>
#include <omg/bit_helper.h>
#include <omg/code_guard.h>

namespace Ion {
namespace ExamMode {

using Int = ExamBytes::Int;

CODE_GUARD(exam_mode_rulesets, 2664271436,  //
           enum class Ruleset
           : Int{
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
           };)

// Number of exam modes (TODO: -1 for Uninitialized)
static constexpr size_t k_numberOfModes =
    static_cast<size_t>(Ruleset::NumberOfRulesets);

// Number of active exam modes (-2 for Off and PressToTest)
static constexpr size_t k_numberOfActiveModes = k_numberOfModes - 2;

/* Encode exam mode permissions on a 16 bits integer.
 * The first bit "configurable" is set to 1 if "data" encodes press-to-test
 * flags. If it set to 0, "data" contains the index of the preset rules (Off,
 * Standard, Dutch...)
 * The bit "clearBit" is always 0 for valid configurations, to ensure
 * compatibility with the PersistingBytes API.
 */
class Configuration {
  CODE_GUARD(exam_mode_configuration, 3023708496,  //
             enum class Bits
             : size_t{
                 Configurable = 0,
                 DataFirst,
                 DataLast = 14,
                 Cleared,
                 NumberOfBits,
             };)

 public:
  constexpr static size_t k_dataSize = static_cast<size_t>(Bits::DataLast) -
                                       static_cast<size_t>(Bits::DataFirst) + 1;

  explicit Configuration(Ruleset rules, Int flags = 0);
  explicit Configuration(Int raw) : m_bits(raw) {}

  bool operator==(const Configuration&) const = default;

  Ruleset ruleset() const;
  Int flags() const;
  Int raw() const { return m_bits; }
  bool isUninitialized() const;
  bool isActive() const;
  KDColor color() const;

 private:
  static_assert(static_cast<int>(Bits::NumberOfBits) ==
                OMG::BitHelper::numberOfBitsIn<Int>());

  bool configurable() const {
    return OMG::BitHelper::bitAtIndex(m_bits, Bits::Configurable);
  }
  Int data() const {
    return OMG::BitHelper::bitsBetweenIndexes(m_bits, Bits::DataLast,
                                              Bits::DataFirst);
  }

  Int m_bits;
};

static_assert(
    sizeof(Configuration) == sizeof(Int),
    "ExamMode::Configuration size not compatible with PersistingBytes");

Configuration get();
void set(Configuration);

}  // namespace ExamMode
}  // namespace Ion

#endif
