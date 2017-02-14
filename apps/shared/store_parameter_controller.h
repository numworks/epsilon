#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <escher.h>
#include "float_pair_store.h"

namespace Shared {

class StoreParameterController : public ViewController, public SimpleListViewDataSource {
public:
  StoreParameterController(Responder * parentResponder, FloatPairStore * store);
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
  FloatPairStore * m_store;
  bool m_xColumnSelected;
};

}

#endif
