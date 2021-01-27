#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/view_controller.h>
#include "function_store.h"

namespace Shared {

class ListParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::SelectableTableViewDelegate * tableDelegate = nullptr);

  Escher::View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ListParameter");
  void setRecord(Ion::Storage::Record record);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  // SimpleListViewDataSource
  int numberOfRows() const override { return k_localNumberOfCell; }
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
protected:
  virtual bool handleEnterOnRow(int rowIndex);
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  int localIndex(int j);
  constexpr static int k_localNumberOfCell = 2;
  Escher::MessageTableCellWithSwitch m_enableCell;
  Escher::MessageTableCell m_deleteCell;
};

}

#endif
