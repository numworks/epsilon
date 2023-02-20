#include <ion.h>
#include <quiz.h>

using namespace Ion::ExamMode;

QUIZ_CASE(ion_exam_mode_uninitialized) {
  Configuration config(static_cast<Int>(-1));
  quiz_assert(config.isUninitialized());
}

QUIZ_CASE(ion_exam_mode) {
  constexpr Int numberOfModes = static_cast<Int>(Mode::NumberOfModes);
  for (Int i = 0; i < numberOfModes; i++) {
    Mode mode = static_cast<Mode>(i);
    set(Configuration(mode));
    Configuration config = get();

    quiz_assert(!config.isUninitialized());
    quiz_assert(config.mode() == mode);
    quiz_assert(config.flags() == 0);
    quiz_assert(config.isActive() == (mode != Mode::Off));
    quiz_assert(config.raw() != static_cast<Int>(-1));

    set(Configuration(0));
  }
}
