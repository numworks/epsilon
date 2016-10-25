#ifndef GRAPH_VALUES_PARAM_CONTROLLER_H
#define GRAPH_VALUES_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval.h"

namespace Graph {
class ValuesParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ValuesParameterController(Responder * parentResponder, Interval * interval);
  Interval * interval();
  int activeCell();
  void editInterval(bool overwrite, char initialDigit = 0);
  void setIntervalParameterAtIndex(int parameterIndex, float f);

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
  constexpr static int k_totalNumberOfCell = 3;
  Interval * m_interval;
  TextMenuListCell m_intervalStartCell;
  TextMenuListCell m_intervalEndCell;
  TextMenuListCell m_intervalStepCell;
  SelectableTableView m_selectableTableView;
};

}

#endif
