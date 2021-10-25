#ifndef APPS_EXAM_POP_UP_CONTROLLER_H
#define APPS_EXAM_POP_UP_CONTROLLER_H

#include <escher/pop_up_controller.h>
#include "exam_pop_up_controller_delegate.h"
#include "global_preferences.h"

class ExamPopUpController : public Escher::PopUpController {
public:
  ExamPopUpController(ExamPopUpControllerDelegate * delegate);
  void setTargetExamMode(GlobalPreferences::ExamMode mode);
  void setTargetPressToTestParams(GlobalPreferences::PressToTestParams params) { m_targetPressToTestParams = params; }
  GlobalPreferences::ExamMode targetExamMode() const { return m_targetExamMode; }
  GlobalPreferences::PressToTestParams targetPressToTestParams() const { return m_targetPressToTestParams; }
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  constexpr static int k_numberOfLines = 4;
  GlobalPreferences::ExamMode m_targetExamMode;
  GlobalPreferences::PressToTestParams m_targetPressToTestParams;
  ExamPopUpControllerDelegate * m_delegate;
};

#endif
