#include <ion/exam_mode.h>

namespace Ion::ExamMode {

Configuration::Configuration(Ruleset, Int) : Configuration(0) {}

Ruleset Configuration::ruleset() const { return static_cast<Ruleset>(0); }

Int Configuration::flags() const { return 0; }

bool Configuration::isActive() const { return false; }

}  // namespace Ion::ExamMode
