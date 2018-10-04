#ifndef SHARED_STORAGE_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "storage_function_go_to_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"

namespace Shared {

template<class T>
class StorageFunctionCurveParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageFunctionCurveParameterController(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
    ViewController(nullptr),
    m_goToCell(I18n::Message::Goto),
    m_selectableTableView(this, this, this),
    m_function(Ion::Storage::Record())
  {}
  View * view() override {
    return &m_selectableTableView;
  }
  void didBecomeFirstResponder() override {
    if (selectedRow() < 0) {
      selectCellAtLocation(0, 0);
    }
    app()->setFirstResponder(&m_selectableTableView);
  }

  KDCoordinate cellHeight() override {
    return Metric::ParameterCellHeight;
  }

  void setFunction(T * function) {
    m_function = *function;
  }

protected:
  bool handleGotoSelection()  {
    if (m_function.isEmpty()) {
      return false;
    }
    goToParameterController()->setFunction(&m_function);
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->push(goToParameterController());
    return true;
  }

  MessageTableCellWithChevron m_goToCell;
  SelectableTableView m_selectableTableView;
  T m_function;
private:
  virtual StorageFunctionGoToParameterController<T> * goToParameterController() = 0;
};

}

#endif
