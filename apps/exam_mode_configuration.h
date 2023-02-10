#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include <apps/i18n.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace ExamModeConfiguration {

// Settings menu
int numberOfAvailableExamMode();
bool pressToTestExamModeAvailable();
bool testModeAvailable();
Poincare::ExamMode::Mode examModeAtIndex(size_t index);
I18n::Message examModeActivationMessage(size_t index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(Poincare::ExamMode mode);

// Exam mode behaviour
I18n::Message forbiddenAppMessage(Poincare::ExamMode::Mode mode, int line);
bool appIsForbidden(I18n::Message appName);

}  // namespace ExamModeConfiguration

#endif
