#ifndef APPS_EXAM_MODE_CONFIGURATION_H
#define APPS_EXAM_MODE_CONFIGURATION_H

#include "global_preferences.h"
#include "shared/settings_message_tree.h"
#include <apps/i18n.h>

namespace ExamModeConfiguration {

// Settings menu
extern const Shared::SettingsMessageTree s_modelExamChildren[2];
int numberOfAvailableExamMode();
GlobalPreferences::ExamMode examModeAtIndex(int index);
I18n::Message examModeActivationMessage(int index);

// Settings pop-up
I18n::Message examModeActivationWarningMessage(GlobalPreferences::ExamMode mode, int line);

// Exam mode behaviour
KDColor examModeColor(GlobalPreferences::ExamMode mode);
bool appIsForbiddenInExamMode(I18n::Message appName, GlobalPreferences::ExamMode mode);
bool exactExpressionsAreForbidden(GlobalPreferences::ExamMode mode);

}

#endif

