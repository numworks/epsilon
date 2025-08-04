#ifndef APPS_EXAM_MODE_MANAGER_H
#define APPS_EXAM_MODE_MANAGER_H

#include <apps/i18n.h>
#include <escher/layout_preferences.h>
#include <ion.h>
#include <kandinsky/font.h>
#include <omg/global_box.h>

class ExamModeManager {
  friend OMG::GlobalBox<ExamModeManager>;
  friend PreferencesTestBuilder;
  friend Ion::Storage::FileSystem;

 public:
  /* Legacy name, inherited from the old Poincare::Preferences object, not
   * changed to preserve website retro-compatibility */
  constexpr static char k_recordName[] = "pr";
  // bool operator==(const Instance&) const = default;

  static void Init() {
    Ion::Storage::FileSystem::sharedFileSystem
        ->initSystemRecord<ExamModeManager>();
    s_examModeManagerInstance = Ion::Storage::FileSystem::sharedFileSystem
                                    ->findSystemRecord<ExamModeManager>();
  }

  static bool ForceExamModeReload() {
    return s_examModeManagerInstance->m_forceExamModeReload;
  }

  static const Poincare::ExamMode* ExamModePtr() {
    return &s_examModeManagerInstance->m_examMode;
  }
  static Poincare::ExamMode ExamMode() {
    return s_examModeManagerInstance->m_examMode;
  };

  static void SetExamMode(Poincare::ExamMode examMode) {
    s_examModeManagerInstance->m_forceExamModeReload = false;
    s_examModeManagerInstance->m_examMode = examMode;
    Ion::ExamMode::set(examMode);
  };

 private:
  constexpr static uint8_t k_version = 1;
  static ExamModeManager* s_examModeManagerInstance;

  /* ExamModeManager is a singleton, hence the private constructor. The unique
   * instance can be accessed through the ExamModeManager::Shared()
   * pointer. */
  ExamModeManager() : m_examMode(Poincare::ExamMode(Ion::ExamMode::get())){};

  CODE_GUARD(poincare_preferences, 125945507,  //
             uint8_t m_version = k_version;
             mutable Poincare::ExamMode m_examMode =
                 Poincare::ExamMode(Ion::ExamMode::Ruleset::Uninitialized);
             /* This flag can only be asserted by writing it via DFU. When set,
              * it will force the reactivation of the exam mode after leaving
              * DFU to synchronize the persisting bytes with the Preferences. */
             bool m_forceExamModeReload = false;)
};

#endif
