#ifndef GRAPH_VALUES_PARAM_CONTROLLER_H
#define GRAPH_VALUES_PARAM_CONTROLLER_H

#include <escher.h>

namespace Graph {
class ValuesParameterController : public ViewController, public ListViewDataSource {
public:
  ValuesParameterController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  TableViewCell m_intervalStartCell;
  TableViewCell m_intervalEndCell;
  TableViewCell m_intervalStepCell;
  ListView m_listView;
  int m_activeCell;
};

}

#endif
