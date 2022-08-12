#ifndef CALCULATION_HISTORY_CONTROLLER_H
#define CALCULATION_HISTORY_CONTROLLER_H

#include "history_view_cell.h"
#include "calculation_store.h"
#include "selectable_table_view.h"
#include "additional_outputs/complex_list_controller.h"
#include "additional_outputs/function_list_controller.h"
#include "additional_outputs/integer_list_controller.h"
#include "additional_outputs/rational_list_controller.h"
#include "additional_outputs/trigonometry_list_controller.h"
#include "additional_outputs/unit_list_controller.h"
#include "additional_outputs/matrix_list_controller.h"
#include "additional_outputs/vector_list_controller.h"

namespace Calculation {

class App;

class HistoryController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource, public Escher::SelectableTableViewDelegate, public HistoryViewCellDataSource {
public:
  HistoryController(EditExpressionController * editExpressionController, CalculationStore * calculationStore);
  Escher::View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  TELEMETRY_ID("");
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void reload();
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedX = -1, int previousSelectedY = -1) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;
  void recomputeHistoryCellHeightsIfNeeded();
private:
  KDCoordinate nonMemoizedRowHeight(int j) override;
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
  TrigonometryListController m_trigonometryController;
  UnitListController m_unitController;
  MatrixListController m_matrixController;
  VectorListController m_vectorController;
  FunctionListController m_functionController;
};

}

#endif
