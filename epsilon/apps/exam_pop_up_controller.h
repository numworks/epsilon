#pragma once

#include <poincare/preferences.h>

#include "shared/pop_up_controller.h"

class ExamPopUpController : public Shared::MessagePopUpController {
 public:
  ExamPopUpController();
  void setTargetExamMode(Poincare::ExamMode mode);
  void viewWillAppear() override;
  void viewDidDisappear() override;

 private:
  constexpr static int k_numberOfLines = 4;

  I18n::Message activationWarningMessage() const;
  bool handleButton() const;

  Poincare::ExamMode m_targetExamMode =
      Poincare::ExamMode(Ion::ExamMode::Ruleset::Uninitialized);
};
