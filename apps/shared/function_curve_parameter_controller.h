#ifndef SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include "function_go_to_parameter_controller.h"
#include <escher/message_table_cell_with_chevron.h>
#include <escher/simple_list_view_data_source.h>

namespace Shared {

class FunctionCurveParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  FunctionCurveParameterController();
  Escher::View * view() override { return &m_selectableTableView; }
  TELEMETRY_ID("CurveParameter");
  void didBecomeFirstResponder() override;
  KDCoordinate rowHeight(int j) override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  bool handleGotoSelection();
  Escher::MessageTableCellWithChevron m_goToCell;
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  virtual FunctionGoToParameterController * goToParameterController() = 0;
};

}

#endif
