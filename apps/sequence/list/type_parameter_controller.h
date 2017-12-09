#ifndef SEQUENCE_TYPE_PARAMATER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMATER_CONTROLLER_H

#include <escher.h>
#include "../sequence_store.h"

namespace Sequence {

class ListController;

class TypeParameterController final : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore, ListController * list,
    TableCell::Layout cellLayout, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  ~TypeParameterController() {
    for (int i = 0; i < k_totalNumberOfCell; i++) {
      delete m_expressionLayouts[i];
      m_expressionLayouts[i] = nullptr;
    }
  }
  TypeParameterController(const TypeParameterController& other) = delete;
  TypeParameterController(TypeParameterController&& other) = delete;
  TypeParameterController& operator=(const TypeParameterController& other) = delete;
  TypeParameterController& operator=(TypeParameterController&& other) = delete;
  const char * title() override;
  View * view() override {
    return &m_selectableTableView;
  }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  void setSequence(Sequence * sequence) {
    m_sequence = sequence;
  }
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 3;
  ExpressionTableCellWithPointer m_expliciteCell;
  ExpressionTableCellWithPointer m_singleRecurrenceCell;
  ExpressionTableCellWithPointer m_doubleRecurenceCell;
  Poincare::ExpressionLayout * m_expressionLayouts[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  SequenceStore * m_sequenceStore;
  Sequence * m_sequence;
  ListController * m_listController;
};

}

#endif
