#ifndef SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include "with_record.h"
#include "function_go_to_parameter_controller.h"
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_list_view_controller.h>

namespace Shared {

class FunctionCurveParameterController : public Escher::SelectableListViewController<Escher::SimpleListViewDataSource>, public WithRecord {
public:
  FunctionCurveParameterController();
  TELEMETRY_ID("CurveParameter");
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  bool handleGotoSelection();
  Escher::MessageTableCellWithChevron m_goToCell;
private:
  virtual FunctionGoToParameterController * goToParameterController() = 0;
};

}

#endif
