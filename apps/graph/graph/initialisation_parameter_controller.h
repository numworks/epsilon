#ifndef GRAPH_GRAPH_INITIALISATION_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_INITIALISATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../../shared/interactive_curve_view_range.h"

namespace Graph {

class InitialisationParameterController : public ViewController, public SimpleListViewDataSource {
public:
  InitialisationParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCells = 4;
  MenuListCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  InteractiveCurveViewRange * m_graphRange;
};

}

#endif
