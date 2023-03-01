#ifndef ION_EXAM_MODE_H
#define ION_EXAM_MODE_H

#include <ion/persisting_bytes.h>
#include <kandinsky/color.h>

namespace Ion {
namespace ExamMode {

using Int = PersistingBytes::PersistingBytesInt;

enum class Ruleset : Int {
  Off = 0,
  Standard,
  Dutch,
  IBTest,
  PressToTest,
  Portuguese,
  English,
  STAAR,
  Keystone,
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
 public:
  explicit Configuration(Ruleset rules, Int flags = 0);
  Configuration() : Configuration(-1) {}
  Configuration(Int raw) : m_internals{.raw = raw} {}

  bool operator==(const Configuration& other) const {
    return m_internals.raw == other.m_internals.raw;
  }

  Ruleset ruleset() const;
  Int flags() const;
  Int raw() const { return m_internals.raw; }
  bool isUninitialized() const;
  bool isActive() const;
  KDColor color() const;

 private:
  union {
    Int raw;
    struct {
      bool configurable : 1;
      Int data : 14;
      bool clearBit : 1;
    } fields;
  } m_internals;
};

static_assert(
    sizeof(Configuration) == sizeof(Int),
    "ExamMode::Configuration size not compatible with PersistingBytes");

Configuration get();
void set(Configuration);

}  // namespace ExamMode
}  // namespace Ion

#endif
