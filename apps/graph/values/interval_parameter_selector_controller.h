#ifndef GRAPH_INTERVAL_PARAMETER_SELECTOR_CONTROLLER
#define GRAPH_INTERVAL_PARAMETER_SELECTOR_CONTROLLER

#include <escher.h>

namespace Graph {

class IntervalParameterSelectorController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  IntervalParameterSelectorController();
  const char * title() override;
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  int reusableCellCount() override;
  HighlightCell * reusableCell(int index) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  static constexpr int MaxNumberOfRows = 1;
  I18n::Message messageAtIndex(int index);
  MessageTableCellWithChevron m_intervalParameterCell[MaxNumberOfRows];
  SelectableTableView m_selectableTableView;
};

}

#endif
