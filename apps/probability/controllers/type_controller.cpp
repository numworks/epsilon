#include "type_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include <poincare/print.h>

#include "hypothesis_controller.h"
#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

void TypeView::layoutSubviews(bool force) {
  /* SelectableTableView must be given a width before computing height. */
  m_list->initWidth(m_frame.width());
  KDSize listSize = m_list->minimalSizeForOptimalDisplay();
  m_list->setFrame(KDRect(KDPointZero, listSize), force);
  m_description->setFrame(
      KDRect(0, listSize.height(), m_frame.width(), m_frame.height() - listSize.height()),
      force);
}

TypeController::TypeController(StackViewController * parent,
                               HypothesisController * hypothesisController,
                               InputController * inputController,
                               Statistic * statistic) :
      Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>(parent),
      m_hypothesisController(hypothesisController),
      m_inputController(inputController),
      m_contentView(&m_selectableTableView, &m_description),
      m_statistic(statistic) {
  m_description.setBackgroundColor(Palette::WallScreen);
  m_description.setTextColor(Palette::GrayDark);
  m_description.setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  m_description.setFont(KDFont::SmallFont);
  m_selectableTableView.setBottomMargin(0);

  // Init selection
  selectRow(0);
}

void TypeController::didBecomeFirstResponder() {
  m_description.setMessage(m_statistic->distributionDescription());
  resetMemoization();
  m_selectableTableView.reloadData(true);
  m_contentView.layoutSubviews();
}

bool TypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    DistributionType type;
    int selRow = selectedRow();
    if (selRow == k_indexOfTTest) {
      type = DistributionType::T;
    } else if (selRow == indexOfZTest()) {
      type = DistributionType::Z;
    } else {
      assert(selRow == k_indexOfPooledTest);
      type = DistributionType::TPooled;
    }
    Escher::ViewController * controller = m_inputController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
    assert(controller != nullptr);
    m_statistic->initializeDistribution(type);
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

Escher::View * TypeView::subviewAtIndex(int i) {
  Escher::View * views[] = {m_list, m_description};
  assert(i >= 0 && i < sizeof(views)/sizeof(Escher::View *));
  return views[i];
}

const char * TypeController::title() {
  I18n::Message format = m_statistic->distributionTitle();
  I18n::Message testOrInterval = m_statistic->statisticBasicTitle();
  Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), I18n::translate(format), I18n::translate(testOrInterval));
  return m_titleBuffer;
}

void TypeController::stackOpenPage(Escher::ViewController * nextPage) {
  switch (m_statistic->distributionType()) {
    case DistributionType::T:
      selectRow(k_indexOfTTest);
      break;
    case DistributionType::TPooled:
      selectRow(k_indexOfPooledTest);
      break;
    default:
      assert(m_statistic->distributionType() == DistributionType::Z);
      selectRow(indexOfZTest());
      break;
  }
  ViewController::stackOpenPage(nextPage);
}

int Probability::TypeController::numberOfRows() const {
  return m_statistic->numberOfAvailableDistributions();
}

void Probability::TypeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  assert(i <= indexOfZTest());
  I18n::Message message, submessage;
  if (i == k_indexOfTTest) {
    message = m_statistic->tDistributionName();
    submessage = I18n::Message::Recommended;
  } else if (i == indexOfZTest()) {
    message = m_statistic->zDistributionName();
    submessage = I18n::Message::RarelyUsed;
  } else {
    assert(i == k_indexOfPooledTest);
    message = m_statistic->tPooledDistributionName();
    submessage = I18n::Message::RarelyUsed;
  }
  Escher::MessageTableCellWithChevronAndMessage * mcell =
    static_cast<Escher::MessageTableCellWithChevronAndMessage *>(cell);
  mcell->setMessage(message);
  mcell->setSubtitle(submessage);
}


}
