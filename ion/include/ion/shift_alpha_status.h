#ifndef ION_SHIFT_ALPHA_STATUS_H
#define ION_SHIFT_ALPHA_STATUS_H

#include <stdint.h>

namespace Ion {
namespace Events {

class ShiftAlphaStatus {
 public:
  enum class ShiftStatus : bool { Inactive = false, Active = true };
  enum class AlphaStatus : uint8_t { Inactive = 0, Active = 1, Locked = 2 };
  constexpr static int k_numberOfAlphaStatus = 3;

  constexpr ShiftAlphaStatus(ShiftStatus shiftStatus, AlphaStatus alphaStatus)
      : m_value(0) {
    updateValue(shiftStatus, alphaStatus);
  }

  constexpr ShiftAlphaStatus()
      : ShiftAlphaStatus(ShiftStatus::Inactive, AlphaStatus::Inactive) {}

  operator uint8_t() const { return m_value; }

  bool operator==(const ShiftAlphaStatus& other) {
    return m_value == other.m_value;
  }
  bool operator!=(const ShiftAlphaStatus& other) { return !(*this == other); }

  bool shiftIsActive() const { return shiftStatus() != ShiftStatus::Inactive; }
  bool alphaIsActive() const { return alphaStatus() != AlphaStatus::Inactive; }
  bool alphaIsLocked() const { return alphaStatus() == AlphaStatus::Locked; }

  void removeShift() { updateValue(ShiftStatus::Inactive, alphaStatus()); }
  void removeAlpha() { updateValue(shiftStatus(), AlphaStatus::Inactive); }

  void toggleShift() {
    updateValue(static_cast<ShiftStatus>(!static_cast<bool>(shiftStatus())),
                alphaStatus());
  }
  void cycleAlpha() {
    uint8_t nextAlphaStatus = static_cast<uint8_t>(alphaStatus()) + 1;
    if (nextAlphaStatus >= k_numberOfAlphaStatus) {
      m_value = 0;
      return;
    }
    updateValue(shiftStatus(), static_cast<AlphaStatus>(nextAlphaStatus));
  }

 private:
  constexpr void updateValue(ShiftStatus shiftStatus, AlphaStatus alphaStatus) {
    m_value = (static_cast<uint8_t>(alphaStatus) << 1) |
              static_cast<uint8_t>(shiftStatus);
  }

  ShiftStatus shiftStatus() const {
    return static_cast<ShiftStatus>(m_value & 0b001);
  }
  AlphaStatus alphaStatus() const {
    return static_cast<AlphaStatus>(m_value >> 1);
  }

  /* uint8_t m_value = 00000AAS
   * Right-most bit is shiftStatus (S), 2 bits left of it is alpha status (A).
   * This is implemented as an uint8_t so that only 1 register has to be passed
   * through SVCalls. */
  uint8_t m_value;
};

}  // namespace Events
}  // namespace Ion
#endif
