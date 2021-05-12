#include "homogeneity_results_controller.h"

#include <apps/i18n.h>
#include <escher/palette.h>
#include <kandinsky/font.h>

using namespace Probability;

HomogeneityResultsView::HomogeneityResultsView(Escher::Responder * parent, Escher::SelectableTableView * table)
    : m_title(KDFont::SmallFont, I18n::Message::HomogeneityResultsTitle),
      m_table(table),
      m_next(parent, I18n::Message::Next, buttonActionInvocation()) {
  setView(&m_title, 0);
  setView(m_table, 1);
  setView(&m_next, 2);
}

void HomogeneityResultsView::buttonAction() {}

HomogeneityResultsController::HomogeneityResultsController(StackViewController * stackViewController)
    : Page(stackViewController),
      m_topLeftCell(Escher::Palette::GrayMiddle),
      m_contentView(this, &m_table), m_table(this, this, this) {
  // Row header
  for (int i = 0; i < k_initialNumberOfColumns; i++) {
    m_rowHeader[i].setFont(KDFont::SmallFont);
    m_rowHeader[i].setText("Category");
  }
  // Column header
  for (int i = 0; i < k_initialNumberOfRows; i++) {
    m_colHeader[i].setFont(KDFont::SmallFont);
    m_colHeader[i].setText("Category");
  }
  // Result cell
  for (int i = 0; i < k_maxNumberOfResultCells; i++) {
    m_cells[i].setFont(KDFont::SmallFont);
    m_cells[i].setText("1");
  }
}

HighlightCell * HomogeneityResultsController::reusableCell(int i, int type) {
  if (i == 0) {
    return &m_topLeftCell;
  }
  if (i < numberOfColumns()) {
    return &m_colHeader[i - 1];
  }
  if (i % numberOfColumns() == 0) {
    return &m_rowHeader[i / numberOfColumns() - 1];
  }
  int index = indexForEditableCell(i);
  return &m_cells[index];
}

int HomogeneityResultsController::indexForEditableCell(int i) {
  // Substracts the number of cells in top and left header before this index
  int row = i / numberOfColumns();
  return i - row - numberOfColumns() + 1;
}
