#ifndef PROBABILITY_LAW_CONTROLLER_H
#define PROBABILITY_LAW_CONTROLLER_H

#include <escher.h>

namespace Probability {

class LawController : public ViewController, public SimpleListViewDataSource {
public:
  LawController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  int numberOfRows() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfModels = 7;
  constexpr static int k_maxNumberOfCells = 10;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the listview!
  MenuListCell m_cells[k_maxNumberOfCells];
  SelectableTableView m_selectableTableView;
  const char ** m_messages;
};

}

#endif
