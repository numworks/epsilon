#include "graph_type_controller.h"
#include <apps/i18n.h>
#include <assert.h>
#include "../box_icon.h"
#include "../histogram_icon.h"
#include "../cumulative_icon.h"
#include "../statistics_normal_icon.h"

using namespace Statistics;

GraphTypeController::GraphTypeController(Escher::Responder * parentResponder,
                               Escher::Responder * tabController,
                               Escher::StackViewController * stackView,
                               Store * store,
                               int * selectedGraphViewIndex) :
    Escher::SelectableCellListPage<Escher::TransparentImageWithMessageCell, k_numberOfCells, Escher::RegularListViewDataSource>(parentResponder),
    m_tabController(tabController),
    m_stackView(stackView),
    m_store(store),
    m_selectedGraphViewIndex(selectedGraphViewIndex) {
  selectRow(*selectedGraphViewIndex);
  // TODO : Translate texts
  cellAtIndex(k_indexOfHistogram)->setMessage(I18n::Message::Histogram);
  cellAtIndex(k_indexOfHistogram)->setImage(ImageStore::HistogramIcon);
  cellAtIndex(k_indexOfBox)->setMessage(I18n::Message::BoxAndWhiskers);
  cellAtIndex(k_indexOfBox)->setImage(ImageStore::BoxIcon);
  cellAtIndex(k_indexOfCumulative)->setMessage(I18n::Message::CumulativeFrequency);
  cellAtIndex(k_indexOfCumulative)->setImage(ImageStore::CumulativeIcon);
  cellAtIndex(k_indexOfNormal)->setMessage(I18n::Message::NormalProbabilityPlot);
  cellAtIndex(k_indexOfNormal)->setImage(ImageStore::StatisticsNormalIcon);
}

void GraphTypeController::didBecomeFirstResponder() {
  selectRow(*m_selectedGraphViewIndex);
  m_selectableTableView.reloadData(true);
}

bool GraphTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up && selectedRow() == 0) {
    Escher::Container::activeApp()->setFirstResponder(m_tabController);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    *m_selectedGraphViewIndex = selectedRow();
    m_stackView->pop();
    return true;
  }
  return false;
}
