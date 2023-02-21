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
  NumberOfRulesets,
};

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
