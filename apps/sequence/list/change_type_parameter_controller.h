#ifndef SEQUENCE_CHANGE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_CHANGE_TYPE_PARAMATER_CONTROLLER_H

#include <escher.h>
#include "../sequence_store.h"

namespace Sequence {

class ChangeTypeParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ChangeTypeParameterController(Responder * parentResponder);
  ~ChangeTypeParameterController();
  const char * title() const override;
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  void setSequence(Sequence * sequence);
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 3;
  ExpressionMenuListCell m_expliciteCell;
  ExpressionMenuListCell m_singleRecurrenceCell;
  ExpressionMenuListCell m_doubleRecurenceCell;
  Poincare::ExpressionLayout * m_expressionLayouts[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  Sequence * m_sequence;
};

}

#endif
