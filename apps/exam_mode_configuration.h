#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include <poincare/preferences.h>
#include <apps/i18n.h>
#include <poincare/expression.h>

namespace ExamModeConfiguration {

// Settings menu
int numberOfAvailableExamMode();
bool pressToTestExamModeAvailable();
bool testModeAvailable();
Poincare::Preferences::ExamMode examModeAtIndex(size_t index);
I18n::Message examModeActivationMessage(size_t index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(Poincare::Preferences::ExamMode mode);

// Exam mode behaviour
I18n::Message forbiddenAppMessage(Poincare::Preferences::ExamMode mode, int line);
KDColor examModeColor(Poincare::Preferences::ExamMode mode);
bool appIsForbidden(I18n::Message appName);
bool exactExpressionIsForbidden(Poincare::Expression e);
bool additionalResultsAreForbidden();
bool lineDetailsAreForbidden();
bool inequalityGraphingIsForbidden();
bool implicitPlotsAreForbidden();
bool statsDiagnosticsAreForbidden();
bool vectorProductsAreForbidden();
bool vectorNormIsForbidden();
bool basedLogarithmIsForbidden();
bool sumIsForbidden();
bool unitsAreForbidden();

// Title bar
I18n::Message examModeTitleBarMessage(Poincare::Preferences::ExamMode mode);

}

#endif

