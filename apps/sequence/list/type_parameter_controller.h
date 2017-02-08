#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher.h>
#include "../sequence_store.h"

namespace Sequence {

class TypeParameterController : public ViewController, public SimpleListViewDataSource {
public:
  TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore);
  const char * title() const override;
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 3;
  TextBufferMenuListCell m_expliciteCell;
  TextBufferMenuListCell m_singleRecurrenceCell;
  TextBufferMenuListCell m_doubleRecurenceCell;
  SelectableTableView m_selectableTableView;
  SequenceStore * m_sequenceStore;
};

}

#endif
