#ifndef APPS_EXAM_POP_UP_CONTROLLER_H
#define APPS_EXAM_POP_UP_CONTROLLER_H

#include <escher/pop_up_controller.h>
#include "exam_pop_up_controller_delegate.h"
#include "global_preferences.h"

class ExamPopUpController : public PopUpController {
public:
  ExamPopUpController(ExamPopUpControllerDelegate * delegate);
  void setTargetExamMode(GlobalPreferences::ExamMode mode);
  GlobalPreferences::ExamMode targetExamMode() const { return m_targetExamMode; }
  void viewDidDisappear() override;
private:
  constexpr static int k_numberOfLines = 3;
  GlobalPreferences::ExamMode m_targetExamMode;
  ExamPopUpControllerDelegate * m_delegate;
};

#endif
