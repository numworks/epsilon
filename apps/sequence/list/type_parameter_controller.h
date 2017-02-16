#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher.h>
#include "../sequence_store.h"

namespace Sequence {

class TypeParameterController : public ViewController, public SimpleListViewDataSource {
public:
  TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore,
    KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0,
    KDCoordinate leftMargin = 0);
  ~TypeParameterController();
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
  TextExpressionMenuListCell m_expliciteCell;
  TextExpressionMenuListCell m_singleRecurrenceCell;
  TextExpressionMenuListCell m_doubleRecurenceCell;
  Poincare::ExpressionLayout * m_expressionLayouts[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  SequenceStore * m_sequenceStore;
  Sequence * m_sequence;
};

}

#endif
