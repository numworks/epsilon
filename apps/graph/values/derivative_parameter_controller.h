#ifndef GRAPH_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/selectable_list_view_controller.h>
#include "../continuous_function_store.h"

namespace Graph {

class ValuesController;

class DerivativeParameterController : public Escher::SelectableListViewController {
public:
  DerivativeParameterController(ValuesController * valuesController);

  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void setRecord(Ion::Storage::Record record) {
    m_record = record;
  }
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
  Ion::Storage::Record m_record;
  ValuesController * m_valuesController;
};

}

#endif
