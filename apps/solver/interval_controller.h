#ifndef SOLVER_INTERVAL_CONTROLLER_H
#define SOLVER_INTERVAL_CONTROLLER_H

#include <apps/shared/single_range_controller.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "equation_store.h"

namespace Solver {

class IntervalController : public Shared::SingleRangeController<double> {
 public:
  IntervalController(Escher::Responder* parentResponder);
  const char* title() const override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  I18n::Message parameterMessage(int index) const override;
  double limit() const override;
  void extractParameters() override;
  void confirmParameters() override;
  bool parametersAreDifferent() override;
  void setAutoRange() override;
  void pop(bool onConfirmation) override;
  Shared::MessagePopUpController m_confirmPopUpController;
};

}  // namespace Solver

#endif
