#ifndef APPS_EXAM_POP_UP_CONTROLLER_H
#define APPS_EXAM_POP_UP_CONTROLLER_H

#include <poincare/preferences.h>

#include "shared/pop_up_controller.h"

class ExamPopUpController : public Shared::MessagePopUpController {
 public:
  ExamPopUpController();
  void setTargetExamMode(Poincare::Preferences::ExamMode mode);
  void setTargetPressToTestParams(
      Poincare::Preferences::PressToTestParams params) {
    m_targetPressToTestParams = params;
  }
  Poincare::Preferences::ExamMode targetExamMode() const {
    return m_targetExamMode;
  }
  Poincare::Preferences::PressToTestParams targetPressToTestParams() const {
    return m_targetPressToTestParams;
  }
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  constexpr static int k_numberOfLines = 4;
  Poincare::Preferences::ExamMode m_targetExamMode;
  Poincare::Preferences::PressToTestParams m_targetPressToTestParams;
};

#endif
