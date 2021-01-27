#ifndef GRAPH_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/view_controller.h>
#include "../continuous_function_store.h"

namespace Graph {

class ValuesController;

class DerivativeParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  DerivativeParameterController(ValuesController * valuesController);

  Escher::View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void setRecord(Ion::Storage::Record record) {
    m_record = record;
  }
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
private:
  ContinuousFunctionStore * functionStore();
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  constexpr static int k_maxNumberOfCharsInTitle = Shared::Function::k_maxNameWithArgumentSize + 1; // +1 for the ' of the derivative
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  Escher::MessageTableCell m_hideColumn;
#if COPY_COLUMN
  Escher::MessageTableCellWithChevron m_copyColumn;
#endif
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  ValuesController * m_valuesController;
};

}

#endif
