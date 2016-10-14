#ifndef GRAPH_VALUES_PARAM_CONTROLLER_H
#define GRAPH_VALUES_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval.h"

namespace Graph {
class ValuesParameterController : public ViewController, public ListViewDataSource {
public:
  ValuesParameterController(Responder * parentResponder, Interval * interval);
  Interval * interval();
  TextTableViewCell * tableViewCellAtIndex(int index);
  int activeCell();
  void editParameterInterval();
  void setIntervalParameterAtIndex(int parameterIndex, float f);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(View * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  Interval * m_interval;
  TextTableViewCell m_intervalStartCell;
  TextTableViewCell m_intervalEndCell;
  TextTableViewCell m_intervalStepCell;
  ListView m_listView;
  int m_activeCell;
};

}

#endif
