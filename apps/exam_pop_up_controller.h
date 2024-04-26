#ifndef APPS_EXAM_POP_UP_CONTROLLER_H
#define APPS_EXAM_POP_UP_CONTROLLER_H

#include <poincare/old/preferences.h>

#include "shared/pop_up_controller.h"

class ExamPopUpController : public Shared::MessagePopUpController {
 public:
  ExamPopUpController();
  void setTargetExamMode(Poincare::ExamMode mode);
  Poincare::ExamMode targetExamMode() const { return m_targetExamMode; }
  void viewWillAppear() override;
  void viewDidDisappear() override;

 private:
  constexpr static int k_numberOfLines = 4;

  I18n::Message activationWarningMessage() const;
  bool handleButton() const;

  Poincare::ExamMode m_targetExamMode;
};

#endif
