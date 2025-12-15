#include <ion.h>
#include <quiz.h>

using namespace Ion::ExamMode;

constexpr static Int k_clearedBits = static_cast<Int>(-1);

QUIZ_CASE(ion_exam_mode_uninitialized) {
  Configuration clearedConfig(k_clearedBits);
  quiz_assert(clearedConfig.isUninitialized());
  Configuration defaultConfig(Ruleset::Uninitialized);
  quiz_assert(defaultConfig.isUninitialized());
}

QUIZ_CASE(ion_exam_mode) {
  for (Int i = 0; i < k_numberOfModes; i++) {
    Ruleset rules = toRuleset(i);
    if (get() != Configuration(rules)) {
      set(Configuration(rules));
    }
    Configuration config = get();

    quiz_assert(!config.isUninitialized());
    quiz_assert(config.ruleset() == rules);
    quiz_assert(config.flags() == 0);
    quiz_assert(config.isActive() == (rules != Ruleset::Off));
    quiz_assert(config.raw() != k_clearedBits);

    if (rules != Ruleset::Off) {
      set(Configuration(Ruleset::Off));
    }
  }
}
