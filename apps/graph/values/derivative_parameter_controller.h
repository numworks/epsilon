#ifndef GRAPH_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher.h>
#include "../continuous_function_store.h"

namespace Graph {

class ValuesController;

class DerivativeParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  DerivativeParameterController(ValuesController * valuesController);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const  override;
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
  MessageTableCell<> m_hideColumn;
#if COPY_COLUMN
  MessageTableCellWithChevron<> m_copyColumn;
#endif
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  ValuesController * m_valuesController;
};

}

#endif
