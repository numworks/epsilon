#ifndef SOLVER_INTERVAL_CONTROLLER_H
#define SOLVER_INTERVAL_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "equation_store.h"

namespace Solver {

class IntervalController : public Shared::FloatParameterController<double> {
 public:
  IntervalController(Escher::Responder* parentResponder);
  const char* title() override;
  TELEMETRY_ID("Interval");
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;

 private:
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  constexpr static int k_maxNumberOfCells = 2;
  Escher::MessageTextView m_instructions;
  Escher::MenuCellWithEditableText<Escher::MessageTextView>
      m_intervalCell[k_maxNumberOfCells];
};

}  // namespace Solver

#endif
