#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <apps/calculation/additional_results/additional_results_controller.h>

#include "calculation_selectable_list_view.h"
#include "calculation_store.h"
#include "history_view_cell.h"

namespace Calculation {

class App;

class HistoryController : public Escher::ViewController,
                          public Escher::ListViewDataSource,
                          public Escher::SelectableListViewDataSource,
                          public Escher::SelectableListViewDelegate,
                          public HistoryViewCellDataSource {
 public:
  HistoryController(EditExpressionController* editExpressionController,
                    CalculationStore* calculationStore);
  Escher::View* view() override { return &m_selectableListView; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  void handleResponderChainEvent(ResponderChainEvent event) override;
  void reload();
  int numberOfRows() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  void setSelectedSubviewType(SubviewType subviewType, bool sameCell,
                              int previousSelectedRow = -1) override;
  void listViewDidChangeSelectionAndDidScroll(
      Escher::SelectableListView* list, int previousSelectedRow,
      KDPoint previousOffset, bool withinTemporarySelection = false) override;
  KDPoint offsetToRestoreAfterReload(
      const Escher::SelectableTableView* t) const override;
  void recomputeHistoryCellHeightsIfNeeded();

 private:
  KDCoordinate nonMemoizedRowHeight(int row) override;

  int storeIndex(int i) const { return numberOfRows() - i - 1; }
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int i) const;
  bool calculationAtIndexToggles(int index) const;
  void handleOK();

  constexpr static int k_maxNumberOfDisplayedRows = 6;

  CalculationSelectableListView m_selectableListView;
  HistoryViewCell m_calculationHistory[k_maxNumberOfDisplayedRows];
  CalculationStore* m_calculationStore;
  AdditionalResultsController m_additionalResultsController;
};

}  // namespace Calculation

#endif
