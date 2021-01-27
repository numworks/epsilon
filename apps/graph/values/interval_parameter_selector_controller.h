#ifndef GRAPH_INTERVAL_PARAMETER_SELECTOR_CONTROLLER
#define GRAPH_INTERVAL_PARAMETER_SELECTOR_CONTROLLER

#include <apps/shared/interval_parameter_controller.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/view_controller.h>
#include "../../shared/continuous_function.h"

namespace Graph {

class IntervalParameterSelectorController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  IntervalParameterSelectorController();
  const char * title() override;
  Escher::View * view() override { return &m_selectableTableView; }
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setStartEndMessages(Shared::IntervalParameterController * controller, Shared::ContinuousFunction::PlotType plotType);
private:
  Shared::ContinuousFunction::PlotType plotTypeAtRow(int j) const;
  I18n::Message messageForType(Shared::ContinuousFunction::PlotType plotType);
  Escher::MessageTableCellWithChevron m_intervalParameterCell[Shared::ContinuousFunction::k_numberOfPlotTypes];
  Escher::SelectableTableView m_selectableTableView;
};

}

#endif
