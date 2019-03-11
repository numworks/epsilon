#ifndef SHARED_STORAGE_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "storage_function_go_to_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class FunctionCurveParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  FunctionCurveParameterController(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor);
  View * view() override;
  void didBecomeFirstResponder() override;
  KDCoordinate cellHeight() override;
  void setRecord(Ion::Storage::Record recor);
protected:
  bool handleGotoSelection();
  MessageTableCellWithChevron m_goToCell;
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  virtual FunctionGoToParameterController * goToParameterController() = 0;
};

}

#endif
