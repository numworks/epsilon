#include <poincare/exam_mode.h>

namespace Poincare {

KDColor ExamMode::color() const {
  assert(isValid());

  constexpr KDColor k_standardLEDColor = KDColorRed;
  constexpr KDColor k_portugueseLEDColor = KDColorGreen;
  /* The Dutch exam mode LED is supposed to be orange but, due to a disparity in
   * plastic quality, this color needs to be toned down closer to a yellow so
   * that it cannot be confused with the red color. In addition, light guide
   * tends to filter green light. 0xA1FF00 is a yellowish green, but appear as a
   * orangish yellow on most calculators, and is good enough for all plastic
   * disparities. Note : pure Orange LED is already used when the battery is
   * charging. */
  constexpr KDColor k_dutchLEDColor = KDColor::RGB24(0xA1FF00);

  switch (mode()) {
    case Mode::Standard:
      return k_standardLEDColor;
    case Mode::Dutch:
      return k_dutchLEDColor;
    case Mode::Portuguese:
      return k_portugueseLEDColor;
    default:
      return KDColorBlack;
  }
}

// PersistingBytes interface

using ExamModeInt = Ion::PersistingBytes::PersistingBytesInt;

ExamMode ExamMode::GetFromPersistingBytes() {
  ExamModeInt raw = Ion::PersistingBytes::read();
  ExamMode examMode = *reinterpret_cast<ExamMode *>(&raw);
  if (!examMode.isValid()) {
    /* The persisting bytes do not contain a valid exam mode, most likely
     * because of a botched install. */
    examMode = ExamMode(Mode::Off);
    examMode.setInPersistingBytes();
  }
  return examMode;
}

void ExamMode::setInPersistingBytes() const {
  assert(isValid());
  ExamModeInt raw = *reinterpret_cast<const ExamModeInt *>(this);
  Ion::PersistingBytes::write(raw);
}

}  // namespace Poincare
