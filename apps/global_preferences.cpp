#include "global_preferences.h"

static GlobalPreferences s_globalPreferences;

GlobalPreferences::GlobalPreferences() :
  m_language(I18n::Language::French),
  m_examMode(ExamMode::Desactivate)
{
}

GlobalPreferences * GlobalPreferences::sharedGlobalPreferences() {
  return &s_globalPreferences;
}

I18n::Language GlobalPreferences::language() const {
  return m_language;
}

void GlobalPreferences::setLanguage(I18n::Language language) {
  if (language != m_language) {
    m_language = language;
  }
}

GlobalPreferences::ExamMode GlobalPreferences::examMode() const {
  return m_examMode;
}

void GlobalPreferences::setExamMode(ExamMode examMode) {
  if (examMode != m_examMode) {
    m_examMode = examMode;
  }
}
