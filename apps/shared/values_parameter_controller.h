#ifndef SHARED_VALUES_PARAM_CONTROLLER_H
#define SHARED_VALUES_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval_parameter_controller.h"

namespace Shared {
class ValuesParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ValuesParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController, char symbol);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  constexpr static int k_maxNumberOfCharsInTitle = 10;
  char m_pageTitle[10];
  PointerTableCell m_deleteColumn;
  PointerTableCellWithChevron m_copyColumn;
  PointerTableCellWithChevron m_setInterval;
  SelectableTableView m_selectableTableView;
  IntervalParameterController * m_intervalParameterController;
};

}

#endif
