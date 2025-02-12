#include <ion.h>
#include <quiz.h>

using namespace Ion::ExamMode;

QUIZ_CASE(ion_exam_mode_uninitialized) {
  Configuration config(static_cast<Int>(-1));
  quiz_assert(config.isUninitialized());
}

QUIZ_CASE(ion_exam_mode) {
  for (Int i = 0; i < k_numberOfModes; i++) {
    Ruleset rules = static_cast<Ruleset>(i);
    set(Configuration(rules));
    Configuration config = get();

    quiz_assert(!config.isUninitialized());
    quiz_assert(config.ruleset() == rules);
    quiz_assert(config.flags() == 0);
    quiz_assert(config.isActive() == (rules != Ruleset::Off));
    quiz_assert(config.raw() != static_cast<Int>(-1));

    set(Configuration(0));
  }
}
