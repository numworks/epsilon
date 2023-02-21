#include <ion.h>
#include <quiz.h>

using namespace Ion::ExamMode;

QUIZ_CASE(ion_exam_mode_uninitialized) {
  Configuration config(static_cast<Int>(-1));
  quiz_assert(config.isUninitialized());
}

QUIZ_CASE(ion_exam_mode) {
  constexpr Int numberOfRulesets = static_cast<Int>(Ruleset::NumberOfRulesets);
  for (Int i = 0; i < numberOfRulesets; i++) {
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
