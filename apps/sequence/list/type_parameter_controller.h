#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher.h>
#include "../sequence.h"

namespace Sequence {

class TypeParameterController : public ViewController, public SimpleListViewDataSource {
public:
  TypeParameterController(Responder * parentResponder);
  const char * title() const override;
  View * view() override;
  void didBecomeFirstResponder() override;
  void setSequence(Sequence * sequence);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 3;
  TextBufferMenuListCell m_expliciteCell;
  TextBufferMenuListCell m_singleRecurrenceCell;
  TextBufferMenuListCell m_doubleRecurenceCell;
  SelectableTableView m_selectableTableView;
  Sequence * m_sequence;
};

}

#endif
