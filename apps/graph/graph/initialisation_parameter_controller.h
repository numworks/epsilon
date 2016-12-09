#ifndef GRAPH_GRAPH_INITIALISATION_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_INITIALISATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "graph_window.h"
#include "graph_view.h"

namespace Graph {

class InitialisationParameterController : public ViewController, public SimpleListViewDataSource {
public:
  InitialisationParameterController(Responder * parentResponder, GraphWindow * graphWindow, GraphView * graphView);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCells = 4;
  MenuListCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  GraphWindow * m_graphWindow;
  GraphView * m_graphView;
};

}

#endif
