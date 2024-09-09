#ifndef SHARED_COLUMN_HELPER_H
#define SHARED_COLUMN_HELPER_H

#include <escher/input_view_controller.h>
#include <escher/selectable_table_view.h>

#include "double_pair_store.h"
#include "formula_template_menu_controller.h"
#include "pop_up_controller.h"
#include "store_context.h"

namespace Shared {

class ClearColumnHelper {
 public:
  /* this is an ad hoc value. Most of the time, column name are very short like
   * "X1", "n" or "f(x)" */
  constexpr static int k_maxSizeOfColumnName = 17;

  ClearColumnHelper();

  virtual Escher::SelectableTableView* table() = 0;
  virtual size_t fillColumnName(int column, char* buffer) = 0;
  void presentClearSelectedColumnPopupIfClearable();

 protected:
  virtual int numberOfElementsInColumn(int column) const = 0;
  virtual void setClearPopUpContent();

  Shared::BufferPopUpController m_confirmPopUpController;

 private:
  virtual void clearSelectedColumn() = 0;
  virtual bool isColumnClearable(int column) { return true; }
};

class StoreColumnHelper {
 public:
  StoreColumnHelper(Escher::Responder* responder,
                    Poincare::Context* parentContext,
                    ClearColumnHelper* clearColumnHelper);

  int referencedColumn() { return table()->selectedColumn(); }
  ClearColumnHelper* clearColumnHelper() { return m_clearColumnHelper; }
  void selectColumn(int column) { table()->selectColumn(column); }

  /* Hide series */
  // Return false if the series can't switch hide status because it's invalid
  bool switchSelectedColumnHideStatus();
  bool selectedSeriesIsActive() {
    return store()->seriesIsActive(selectedSeries());
  }
  int selectedSeries() {
    return store()->seriesAtColumn(
        m_clearColumnHelper->table()->selectedColumn());
  }

  /* Sort series */
  virtual void sortSelectedColumn();

  /* Fill with formula */
  void displayFormulaInput();
  void fillFormulaInputWithTemplate(const Poincare::Layout& templateLayout);
  void fillFormulaInputWithFormula(const Poincare::Layout& formulaLayout);
  bool fillColumnWithFormula(const Poincare::Layout& formulaLayout);

  /* Clear series */
  size_t fillColumnNameFromStore(int column, char* buffer) {
    return store()->fillColumnName(store()->seriesAtColumn(column),
                                   store()->relativeColumn(column), buffer);
  }
  size_t clearPopUpText(int column, char* buffer, size_t bufferSize);
  size_t clearCellText(int column, char* buffer, size_t bufferSize);

  virtual Escher::InputViewController* inputViewController() = 0;
  virtual DoublePairStore* store() = 0;
  virtual void reload() { table()->reloadData(); }

 protected:
  Escher::SelectableTableView* table() { return m_clearColumnHelper->table(); }
  void reloadSeriesVisibleCells(int series, int relativeColumn = -1);

 private:
  virtual Poincare::Layout memoizedFormula(int index) {
    return Poincare::Layout();
  }
  virtual void memoizeFormula(const Poincare::Layout& formula, int index) {}
  int formulaMemoizationIndex(int series, int relativeColumn);
  enum class FillColumnStatus : uint8_t {
    Success,
    NoDataToStore,
    SyntaxError,
    DataNotSuitable,
  };
  FillColumnStatus privateFillColumnWithFormula(
      const Poincare::Layout& formulaLayout, int* series, int* column);

  ClearColumnHelper* m_clearColumnHelper;
  /* Fill with formula */
  FormulaTemplateMenuController m_templateController;
  Escher::StackViewController::Default m_templateStackController;
  Poincare::Context* m_parentContext;
};

}  // namespace Shared

#endif
