#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include <apps/i18n.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace ExamModeConfiguration {

constexpr KDColor k_standardLEDColor = KDColorRed;
constexpr KDColor k_portugueseLEDColor = KDColorGreen;
/* The Dutch exam mode LED is supposed to be orange but, due to a disparity in
 * plastic quality, this color needs to be toned down closer to a yellow so that
 * it cannot be confused with the red color. In addition, light guide tends to
 * filter green light. 0xA1FF00 is a yellowish green, but appear as a orangish
 * yellow on most calculators, and is good enough for all plastic disparities.
 * Note : pure Orange LED is already used when the battery is charging. */
constexpr KDColor k_dutchLEDColor = KDColor::RGB24(0xA1FF00);

// Settings menu
int numberOfAvailableExamMode();
bool pressToTestExamModeAvailable();
bool testModeAvailable();
Poincare::Preferences::ExamMode examModeAtIndex(size_t index);
I18n::Message examModeActivationMessage(size_t index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(
    Poincare::Preferences::ExamMode mode);

// Exam mode behaviour
I18n::Message forbiddenAppMessage(Poincare::Preferences::ExamMode mode,
                                  int line);
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

}  // namespace ExamModeConfiguration

#endif
