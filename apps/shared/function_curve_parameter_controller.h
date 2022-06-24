#ifndef SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "function_go_to_parameter_controller.h"

namespace Shared {

class FunctionCurveParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  FunctionCurveParameterController();
  View * view() override { return &m_selectableTableView; }
  TELEMETRY_ID("CurveParameter");
  void didBecomeFirstResponder() override;
  KDCoordinate cellHeight() override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  bool handleGotoSelection();
  MessageTableCellWithChevron<> m_goToCell;
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  virtual FunctionGoToParameterController * goToParameterController() = 0;
};

}

#endif
