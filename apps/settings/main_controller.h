#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>

namespace Settings {

class MainController : public ViewController, public SimpleListViewDataSource {
public:
  MainController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 5;
  ChevronMenuListCell m_cells[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
};

}

#endif
