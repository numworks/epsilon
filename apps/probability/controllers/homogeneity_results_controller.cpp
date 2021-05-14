#include "homogeneity_results_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
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

HomogeneityResultsDataSource::HomogeneityResultsDataSource() {
  for (int i = 0; i < HomogeneityTableDataSource::k_maxNumberOfInnerCells; i++) {
    m_cells[i].setFont(KDFont::SmallFont);
    m_cells[i].setText("1");
  }
}

HighlightCell * HomogeneityResultsDataSource::reusableCell(int i, int type) { return &m_cells[i]; }

HomogeneityResultsController::HomogeneityResultsController(StackViewController * stackViewController)
    : Page(stackViewController),
      m_contentView(this, &m_table),
      m_table(this, &m_tableData, &m_tableData),
      m_tableData(&m_innerTableData) {}

void HomogeneityResultsController::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_table);
}
