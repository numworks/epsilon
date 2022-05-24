#ifndef SHARED_COLUMN_HELPER_H
#define SHARED_COLUMN_HELPER_H

#include "double_pair_store.h"
#include "formula_template_menu_controller.h"
#include "pop_up_controller.h"
#include "store_context.h"
#include <escher/input_view_controller.h>
#include <escher/selectable_table_view.h>

namespace Shared {

class ClearColumnHelper {
friend class StoreColumnHelper;
public:
  // this is an ad hoc value. Most of the time, colum_name are very short like "X1", "n" or "f(x)"
  constexpr static int k_maxSizeOfColumnName = 16;

  ClearColumnHelper();
  void presentClearSelectedColumnPopupIfClearable();
  virtual int fillColumnName(int columnIndex, char * buffer) = 0;
  virtual Escher::SelectableTableView * table() = 0;

protected:
  virtual int numberOfElementsInColumn(int columnIndex) const = 0;
  int fillColumnNameWithMessage(char * buffer, I18n::Message message);

  Shared::BufferPopUpController m_confirmPopUpController;

private:
  virtual void clearSelectedColumn() = 0;
  virtual void setClearPopUpContent();
  virtual bool isColumnClearable(int columnIndex) { return true; }
};

class StoreColumnHelper {
public:
  StoreColumnHelper(Escher::Responder * responder, Poincare::Context * parentContext, ClearColumnHelper * clearColumnHelper);

  int referencedColumn() { return table()->selectedColumn(); }
  ClearColumnHelper * clearColumnHelper() { return m_clearColumnHelper; }

  /* Hide series */
  // Return false if the series can't switch hide status because it's invalid
  bool switchSelectedColumnHideStatus();
  bool selectedSeriesIsValid() { return store()->seriesIsValid(selectedSeries()); }
  int selectedSeries() { return store()->seriesAtColumn(m_clearColumnHelper->table()->selectedColumn()); }

  /* Sort series */
  virtual void sortSelectedColumn();

  /* Fill with formula */
  void displayFormulaInput();
  void fillFormulaInputWithTemplate(Poincare::Layout layout);
  bool fillColumnWithFormula(Poincare::Expression formula);
  bool createExpressionForFillingColumnWithFormula(const char * text);

  /* Clear series */
  int fillColumnNameFromStore(int columnIndex, char * buffer) { return store()->fillColumnName(store()->seriesAtColumn(columnIndex), store()->relativeColumnIndex(columnIndex), buffer); }

  virtual Escher::InputViewController * inputViewController() = 0;
  virtual DoublePairStore * store() = 0;

protected:
  Escher::SelectableTableView * table() { return m_clearColumnHelper->table(); }
  void reloadSeriesVisibleCells(int series, int relativeColumn = -1);

private:
  ClearColumnHelper * m_clearColumnHelper;
  /* Fill with formula */
  FormulaTemplateMenuController m_templateController;
  Escher::StackViewController m_templateStackController;
  Poincare::Context * m_parentContext;
};

}

#endif
