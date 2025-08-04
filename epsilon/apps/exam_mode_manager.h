#ifndef APPS_EXAM_MODE_MANAGER_H
#define APPS_EXAM_MODE_MANAGER_H

#include <apps/i18n.h>
#include <escher/layout_preferences.h>
#include <ion.h>
#include <kandinsky/font.h>
#include <omg/global_box.h>

/* [ExamModeManager] is a singleton, hence the private constructor.
 * Its unique instance lives in Storage.
 *
 * Storage does not enforce alignment. The [packed] attribute ensures the
 * compiler will not emit instructions that require the data to be aligned.
 *
 * The static helper methods are the unique way to interact with the
 * [ExamModeManager]. */
class __attribute__((packed)) ExamModeManager {
  friend OMG::GlobalBox<ExamModeManager>;
  friend Ion::Storage::FileSystem;

 public:
  /* Legacy name, inherited from the old Poincare::Preferences object, not
   * changed to preserve website retro-compatibility */
  constexpr static char k_recordName[] = "pr";

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

  ExamModeManager() : m_examMode(Poincare::ExamMode(Ion::ExamMode::get())){};

  CODE_GUARD(poincare_preferences, 1248687274,  //
             uint8_t m_version = k_version;
             Poincare::ExamMode m_examMode;
             /* This flag can only be asserted by writing it via
              * DFU. When set, it will force the reactivation of
              * the exam mode after leaving DFU to synchronize
              * the persisting bytes with the Preferences. */
             bool m_forceExamModeReload = false;)
};

#endif
