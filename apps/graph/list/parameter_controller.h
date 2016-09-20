#ifndef GRAPH_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "../function.h"
#include "../function_store.h"

class ParameterController : public ViewController, public TableViewDataSource {
public:
  ParameterController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Graph::Function * function);
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfCells() override;
  void willDisplayCellForIndex(View * cell, int index) override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;

private:
  constexpr static int k_totalNumberOfCell = 3;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the tableview!
  TableViewCell m_cells[k_totalNumberOfCell];
  TableView m_tableView;
  const char ** m_messages;
  int m_activeCell;
  Graph::Function * m_function;
  Graph::FunctionStore * m_functionStore;
};

#endif
