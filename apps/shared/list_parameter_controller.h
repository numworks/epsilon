#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/list_view_data_source.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/view_controller.h>
#include "function_store.h"

namespace Shared {

class ListParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::SelectableTableViewDelegate * tableDelegate = nullptr);

  Escher::View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ListParameter");
  void setRecord(Ion::Storage::Record record);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  int numberOfRows() const override { return totalNumberOfCells(); }

  // ListViewDataSource
  KDCoordinate rowHeight(int j) override { return Escher::Metric::ParameterCellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return 1; }
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
protected:
  virtual bool handleEnterOnRow(int rowIndex);
  virtual int totalNumberOfCells() const {
#if FUNCTION_COLOR_CHOICE
    return 3;
#else
    return 2;
#endif
  }
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
#if FUNCTION_COLOR_CHOICE
  Escher::MessageTableCellWithChevron m_colorCell;
#endif
  Escher::MessageTableCellWithSwitch m_enableCell;
  Escher::MessageTableCell m_deleteCell;
};

}

#endif
