#include <poincare/exam_mode.h>

namespace Poincare {

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
