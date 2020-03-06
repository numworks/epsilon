#ifndef SHARED_INITIALISATION_PARAMETER_CONTROLLER_H
#define SHARED_INITIALISATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include <apps/i18n.h>

namespace Shared {

class InitialisationParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  InitialisationParameterController(Responder * parentResponder, Shared::InteractiveCurveViewRange * graphRange) :
    ViewController(parentResponder),
    m_selectableTableView(this, this, this),
    m_graphRange(graphRange)
  {}
  View * view() override;
  const char * title() override;
  TELEMETRY_ID("Initialization");
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCells = 4;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  InteractiveCurveViewRange * m_graphRange;
};

}

#endif
