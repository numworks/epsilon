#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include <escher.h>
#include "history_view_cell.h"
#include "calculation_store.h"
#include "selectable_table_view.h"
#include "additional_outputs/complex_list_controller.h"
#include "additional_outputs/integer_list_controller.h"
#include "additional_outputs/rational_list_controller.h"
#include "additional_outputs/second_degree_list_controller.h"
#include "additional_outputs/trigonometry_list_controller.h"
#include "additional_outputs/unit_list_controller.h"
#include "additional_outputs/matrix_list_controller.h"

namespace Calculation {

class App;

class HistoryController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate, public HistoryViewCellDataSource {
public:
  HistoryController(EditExpressionController * editExpressionController, CalculationStore * calculationStore);
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  TELEMETRY_ID("");
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void reload();
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedX = -1, int previousSelectedY = -1) override;
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
  void reinsertTrash() { m_calculationStore->reinsertTrash(); }
private:
  int storeIndex(int i) { return numberOfRows() - i - 1; }
  Shared::ExpiringPointer<Calculation> calculationAtIndex(int i);
  CalculationSelectableTableView * selectableTableView();
  bool calculationAtIndexToggles(int index);
  void historyViewCellDidChangeSelection(HistoryViewCell ** cell, HistoryViewCell ** previousCell, int previousSelectedCellX, int previousSelectedCellY, SubviewType type, SubviewType previousType) override;
  constexpr static int k_maxNumberOfDisplayedRows = 8;
  CalculationSelectableTableView m_selectableTableView;
  HistoryViewCell m_calculationHistory[k_maxNumberOfDisplayedRows];
  CalculationStore * m_calculationStore;
  ComplexListController m_complexController;
  IntegerListController m_integerController;
  RationalListController m_rationalController;
  SecondDegreeListController m_secondDegreeController;
  TrigonometryListController m_trigonometryController;
  UnitListController m_unitController;
  MatrixListController m_matrixController;
};

}

#endif
