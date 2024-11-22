#include "formula_template_menu_controller.h"

#include <ion/storage/record.h>
#include <poincare/k_tree.h>
#include <poincare/print.h>

#include "column_helper.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

FormulaTemplateMenuController::FormulaTemplateMenuController(
    Responder* parentResponder, StoreColumnHelper* storeColumnHelper)
    : SelectableListViewController(parentResponder),
      m_storeColumnHelper(storeColumnHelper) {
  m_emptyTemplateCell.label()->setMessage(I18n::Message::Empty);
  m_selectableListView.resetMargins();
  m_selectableListView.hideScrollBars();
}

const char* FormulaTemplateMenuController::title() {
  return I18n::translate(I18n::Message::FillWithFormula);
}

void FormulaTemplateMenuController::viewWillAppear() {
  computeUninitializedLayouts();
  ViewController::viewWillAppear();
  selectRow(0);
}

void FormulaTemplateMenuController::fillCellForRow(HighlightCell* cell,
                                                   int row) {
  assert(row < k_numberOfTemplates);
  CellType type = static_cast<CellType>(typeAtRow(row));
  if (type == CellType::EmptyTemplate) {
    return;
  }
  computeUninitializedLayouts();
  int index = relativeCellIndex(row, type);
  if (type == CellType::TemplateWithMessage) {
    MessageTemplateCell* myCell = static_cast<MessageTemplateCell*>(cell);
    myCell->label()->setLayout(m_layouts[row - 1]);
    myCell->subLabel()->setMessage(k_subLabelMessages[index]);
    return;
  }
  assert(type == CellType::TemplateWithBuffer);
  BufferTemplateCell* myCell = static_cast<BufferTemplateCell*>(cell);
  myCell->label()->setLayout(m_layouts[row - 1]);
  fillSubLabelBuffer(myCell, index);
}

void FormulaTemplateMenuController::viewDidDisappear() {
  // Reset layouts to clean the pool
  for (int i = 0; i < k_numberOfTemplates - 1; i++) {
    m_layouts[i] = Layout();
  }
  ViewController::viewDidDisappear();
}

bool FormulaTemplateMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int i = selectedRow();
    Layout templateLayout = i == 0 ? Layout() : m_layouts[i - 1];
    App::app()->modalViewController()->dismissModal();
    m_storeColumnHelper->fillFormulaInputWithTemplate(templateLayout);
    return true;
  }
  return false;
}

KDCoordinate FormulaTemplateMenuController::nonMemoizedRowHeight(int row) {
  assert(row < k_numberOfTemplates);
  CellType type = static_cast<CellType>(typeAtRow(row));
  int reusableCellIndex = relativeCellIndex(row, type);
  return protectedNonMemoizedRowHeight(
      reusableCell(reusableCellIndex, typeAtRow(row)), row);
}

HighlightCell* FormulaTemplateMenuController::reusableCell(int index,
                                                           int type) {
  assert(index < reusableCellCount(type));
  CellType cellType = static_cast<CellType>(type);
  if (cellType == CellType::EmptyTemplate) {
    return &m_emptyTemplateCell;
  }
  if (cellType == CellType::TemplateWithMessage) {
    return &m_templatesWithMessage[index];
  }
  assert(cellType == CellType::TemplateWithBuffer);
  return &m_templatesWithBuffer[index];
}

int FormulaTemplateMenuController::reusableCellCount(int type) const {
  if (type == static_cast<int>(CellType::EmptyTemplate)) {
    return 1;
  }
  if (type == static_cast<int>(CellType::TemplateWithMessage)) {
    return k_numberOfExpressionCellsWithMessage;
  }
  assert(type == static_cast<int>(CellType::TemplateWithBuffer));
  return k_numberOfExpressionCellsWithBuffer;
}

int FormulaTemplateMenuController::typeAtRow(int row) const {
  assert(row < numberOfRows());
  if (row <= static_cast<int>(Cell::EmptyTemplate)) {
    return static_cast<int>(CellType::EmptyTemplate);
  }
  if (row <= static_cast<int>(Cell::Logarithm)) {
    return static_cast<int>(CellType::TemplateWithMessage);
  }
  return static_cast<int>(CellType::TemplateWithBuffer);
}

int FormulaTemplateMenuController::relativeCellIndex(int index, CellType type) {
  if (type == CellType::TemplateWithMessage ||
      type == CellType::TemplateWithBuffer) {
    index -= reusableCellCount(static_cast<int>(CellType::EmptyTemplate));
  }
  if (type == CellType::TemplateWithBuffer) {
    index -= reusableCellCount(static_cast<int>(CellType::TemplateWithMessage));
  }
  return index;
}

bool FormulaTemplateMenuController::shouldDisplayOtherAppCell() const {
  char columnName[DoublePairStore::k_columnNamesLength + 1];
  fillOtherAppColumnName(columnName);
  Ion::Storage::Record r(columnName, Ion::Storage::listExtension);
  return r.value().size;
}

UserExpression FormulaTemplateMenuController::templateExpressionForCell(
    Cell cell) {
  assert(cell < Cell::MaxNumberOfRows && cell > Cell::EmptyTemplate);
  if (cell <= Cell::Logarithm) {
    return UserExpression::Parse(k_templates[static_cast<int>(cell) - 1],
                                 nullptr);
  }
  // Build the expression "X2+X3"
  if (cell == Cell::OtherColumns) {
    char name1[DoublePairStore::k_columnNamesLength + 1];
    char name2[DoublePairStore::k_columnNamesLength + 1];
    fillSumColumnNames(name1, name2);
    return UserExpression::Create(
        KAdd(KA, KB), {.KA = SymbolHelper::BuildSymbol(
                           name1, DoublePairStore::k_columnNamesLength),
                       .KB = SymbolHelper::BuildSymbol(
                           name2, DoublePairStore::k_columnNamesLength)});
  }
  // Build the expression "V1"
  assert(cell == Cell::OtherApp && shouldDisplayOtherAppCell());
  char columnName[DoublePairStore::k_columnNamesLength + 1];
  fillOtherAppColumnName(columnName);
  return SymbolHelper::BuildSymbol(columnName,
                                   DoublePairStore::k_columnNamesLength);
}

void FormulaTemplateMenuController::computeUninitializedLayouts() {
  int nRows = numberOfRows();
  for (int i = 1; i < nRows; i++) {
    if (!m_layouts[i - 1].isUninitialized()) {
      continue;
    }
    Poincare::UserExpression e =
        templateExpressionForCell(static_cast<Cell>(i));
    m_layouts[i - 1] =
        e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal,
                       Preferences::ShortNumberOfSignificantDigits,
                       App::app()->localContext());
  }
}

void FormulaTemplateMenuController::fillSubLabelBuffer(BufferTemplateCell* cell,
                                                       int index) {
  I18n::Message message =
      k_subLabelMessages[index + k_numberOfExpressionCellsWithMessage];
  constexpr size_t k_bufferSize =
      Escher::OneLineBufferTextView<>::MaxTextSize();
  char buffer[k_bufferSize];
  if (index == 0) {
    char name1[DoublePairStore::k_columnNamesLength + 1];
    char name2[DoublePairStore::k_columnNamesLength + 1];
    fillSumColumnNames(name1, name2);
    Print::CustomPrintf(buffer, k_bufferSize, I18n::translate(message), name1,
                        name2);
    cell->subLabel()->setText(buffer);
    return;
  }
  assert(index == 1);
  char columnName[DoublePairStore::k_columnNamesLength + 1];
  fillOtherAppColumnName(columnName);
  Print::CustomPrintf(buffer, k_bufferSize, I18n::translate(message),
                      columnName);
  cell->subLabel()->setText(buffer);
}

void replaceSeriesIndex(char* columnName, int value) {
  columnName[1] = '1' + value;
}
int getSeriesIndex(const char* columnName) {
  return static_cast<int>(columnName[1] - '1');
}

void FormulaTemplateMenuController::fillSumColumnNames(
    char* columnName1, char* columnName2) const {
  char currentColumnName[DoublePairStore::k_columnNamesLength + 1];
  m_storeColumnHelper->fillColumnNameFromStore(
      m_storeColumnHelper->referencedColumn(), currentColumnName);
  int currentSeriesIndex = getSeriesIndex(currentColumnName);

  strlcpy(columnName1, currentColumnName,
          DoublePairStore::k_columnNamesLength + 1);
  strlcpy(columnName2, currentColumnName,
          DoublePairStore::k_columnNamesLength + 1);

  /* The displayed sum will be "V_(n-2) + V_(n-1)" (for columnName in the form
   * "V1", "V2", "V3" etc.). For the first and the second column, we display
   * "V2+V3" or "V1+V3" respectively.  */
  static_assert(DoublePairStore::k_numberOfSeries >= 3);
  replaceSeriesIndex(columnName1, currentSeriesIndex == 0 ? 1
                                  : currentSeriesIndex == 1
                                      ? 0
                                      : currentSeriesIndex - 2);
  replaceSeriesIndex(columnName2, currentSeriesIndex == 0 ? 2
                                  : currentSeriesIndex == 1
                                      ? 2
                                      : currentSeriesIndex - 1);
}

char correspondingColumnInOtherApp(char columnPrefix) {
  constexpr static int k_numberOfApps = 2;
  constexpr static const char* const* k_columnNames[k_numberOfApps] = {
      DoublePairStore::k_regressionColumNames,
      DoublePairStore::k_statisticsColumNames};
  for (int i = 0; i < DoublePairStore::k_numberOfColumnsPerSeries; i++) {
    for (int j = 0; j < k_numberOfApps; j++) {
      if (k_columnNames[j][i][0] == columnPrefix) {
        return k_columnNames[(j + 1) % k_numberOfApps][i][0];
      }
    }
  }
  assert(false);
  return 0;
}

void FormulaTemplateMenuController::fillOtherAppColumnName(char* buffer) const {
  m_storeColumnHelper->fillColumnNameFromStore(
      m_storeColumnHelper->referencedColumn(), buffer);
  buffer[0] = correspondingColumnInOtherApp(buffer[0]);
}

}  // namespace Shared
