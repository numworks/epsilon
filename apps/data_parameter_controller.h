#ifndef APPS_DATA_PARAM_CONTROLLER_H
#define APPS_DATA_PARAM_CONTROLLER_H

#include <escher.h>
#include "data.h"

class DataParameterController : public ViewController, public SimpleListViewDataSource {
public:
  DataParameterController(Responder * parentResponder, Data * data);
  void selectXColumn(bool xColumnSelected);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 3;
  MenuListCell m_deleteColumn;
  ChevronMenuListCell m_copyColumn;
  ChevronMenuListCell m_importList;
  SelectableTableView m_selectableTableView;
  Data * m_data;
  bool m_xColumnSelected;
};

#endif
