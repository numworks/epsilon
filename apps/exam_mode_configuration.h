#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include "global_preferences.h"
#include <apps/i18n.h>
#include <poincare/expression.h>

namespace ExamModeConfiguration {

// Settings menu
int numberOfAvailableExamMode();
bool pressToTestExamModeAvailable();
GlobalPreferences::ExamMode examModeAtIndex(int index);
I18n::Message examModeActivationMessage(int index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line);

// Exam mode behaviour
KDColor examModeColor(GlobalPreferences::ExamMode mode);
bool appIsForbidden(I18n::Message appName);
bool exactExpressionIsForbidden(Poincare::Expression e);
bool additionalResultsAreForbidden();
bool inequalityGraphingIsForbidden();
bool implicitPlotsAreForbidden();
bool statsDiagnosticsAreForbidden();
bool vectorsAreForbidden();
bool basedLogarithmIsForbidden();
bool sumIsForbidden();

}

#endif

