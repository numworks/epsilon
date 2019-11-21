#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATED_LIST_CONTROLLER_H

#include <escher.h>
#include "scrollable_input_exact_approximate_expressions_cell.h"
#include "../calculation_store.h"
#include <apps/i18n.h>

namespace Calculation {

class IllustratedListController : public StackViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  IllustratedListController(Responder * parentResponder);

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  //ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  // IllustratedListController
  virtual void fillCalculationStoreFromExpression(Poincare::Expression e);

private:
  class ListController : public ViewController {
  public:
    ListController(IllustratedListController * dataSource);
    const char * title() override { return I18n::translate(I18n::Message::AdditionalResults); }
    View * view() override { return &m_selectableTableView; }
    void didBecomeFirstResponder() override;
    SelectableTableView * selectableTableView() { return &m_selectableTableView; }
  private:
    SelectableTableView m_selectableTableView;
  };
  constexpr static int k_maxNumberOfAdditionalCalculations = 3;
  constexpr static KDCoordinate k_illustrationHeight = 100;
  ListController m_listController;
  // Cells
  virtual HighlightCell * illustrationCell() = 0;
  ScrollableInputExactApproximateExpressionsCell m_additionalCalculationCells[k_maxNumberOfAdditionalCalculations];
  CalculationStore m_calculationStore;
};

}

#endif

