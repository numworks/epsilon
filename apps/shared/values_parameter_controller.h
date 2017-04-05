#ifndef SHARED_VALUES_PARAM_CONTROLLER_H
#define SHARED_VALUES_PARAM_CONTROLLER_H

#include <escher.h>
#include "interval_parameter_controller.h"

namespace Shared {
class ValuesParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ValuesParameterController(Responder * parentResponder, IntervalParameterController * intervalParameterController, I18n::Message title);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  I18n::Message m_pageTitle;
  MessageTableCell m_deleteColumn;
  MessageTableCellWithChevron m_copyColumn;
  MessageTableCellWithChevron m_setInterval;
  SelectableTableView m_selectableTableView;
  IntervalParameterController * m_intervalParameterController;
};

}

#endif
