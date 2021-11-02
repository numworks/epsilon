#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include "global_preferences.h"
#include <apps/i18n.h>
#include <poincare/expression.h>

namespace ExamModeConfiguration {

// Settings menu
int numberOfAvailableExamMode();
GlobalPreferences::ExamMode examModeAtIndex(int index);
I18n::Message examModeActivationMessage(int index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line);

// Exam mode behaviour
KDColor examModeColor(GlobalPreferences::ExamMode mode);
bool appIsForbiddenInExamMode(I18n::Message appName, GlobalPreferences::ExamMode mode);
bool exactExpressionIsForbidden(GlobalPreferences::ExamMode mode, Poincare::Expression e);
bool additionalResultsAreForbidden(GlobalPreferences::ExamMode mode);

}

#endif

