#include "type_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include <poincare/print.h>

#include "hypothesis_controller.h"
#include "probability/app.h"
#include "probability/text_helpers.h"

using namespace Probability;

void Probability::TypeView::layoutSubviews(bool force) {
  m_list->setFrame(KDRect(KDPointZero, m_frame.width(), 0), false);
  KDSize listSize = m_list->minimalSizeForOptimalDisplay();
  m_list->setFrame(KDRect(KDPointZero, listSize), force);
  m_description->setFrame(
      KDRect(0, listSize.height(), m_frame.width(), m_frame.height() - listSize.height()),
      force);
}

TypeController::TypeController(StackViewController * parent,
                               HypothesisController * hypothesisController,
                               InputController * inputController,
                               Data::Test * globalTest,
                               Data::TestType * globalTestType,
                               Statistic * statistic) :
      SelectableListViewPage(parent),
      m_hypothesisController(hypothesisController),
      m_inputController(inputController),
      m_contentView(&m_selectableTableView, &m_description),
      m_globalTestType(globalTestType),
      m_statistic(statistic) {
  m_description.setBackgroundColor(Palette::WallScreen);
  m_description.setTextColor(Palette::GrayDark);
  m_description.setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  m_description.setFont(KDFont::SmallFont);
  m_selectableTableView.setBottomMargin(0);
}

void TypeController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Type);
  m_description.setMessage(messageForTest(App::app()->subapp(), App::app()->test()));
  if (selectedRow() == -1) {
    selectRow(0);
  }
  resetMemoization();
  m_selectableTableView.reloadData(true);
  m_contentView.layoutSubviews();
}

bool TypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::ViewController * view;
    Data::TestType t;
    switch (indexFromListIndex(selectedRow())) {
      case k_indexOfTTest:
        t = Data::TestType::TTest;
        break;
      case k_indexOfPooledTest:
        t = Data::TestType::PooledTTest;
        break;
      case k_indexOfZTest:
        t = Data::TestType::ZTest;
        break;
      default:
        assert(false);
    }
    if (App::app()->subapp() == Data::SubApp::Intervals) {
      view = m_inputController;
    } else {
      view = m_hypothesisController;
    }
    assert(view != nullptr);
    if (t != App::app()->testType()) {
      Statistic::initializeStatistic(m_statistic,
                                     App::app()->subapp(),
                                     App::app()->test(),
                                     t,
                                     Data::CategoricalType::Unset);
    }
    *m_globalTestType = t;
    openPage(view);
    return true;
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

Escher::View * TypeView::subviewAtIndex(int i) {
  switch (i) {
    case 0:
      return m_list;
    case 1:
      return m_description;
    default:
      assert(false);
      return nullptr;
  }
}

const char * TypeController::title() {
  I18n::Message format = App::app()->test() == Data::Test::OneMean
                             ? I18n::Message::TypeControllerTitleOne
                             : I18n::Message::TypeControllerTitleTwo;
  I18n::Message testOrInterval = App::app()->subapp() == Data::SubApp::Tests
                                     ? I18n::Message::Test
                                     : I18n::Message::Interval;
  Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), I18n::translate(format), I18n::translate(testOrInterval));
  return m_titleBuffer;
}

int Probability::TypeController::numberOfRows() const {
  return listIndexFromIndex(k_numberOfRows);
}

void Probability::TypeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  int j = indexFromListIndex(i);
  bool isTestSubApp = App::app()->subapp() == Data::SubApp::Tests;
  if (j < k_indexOfDescription) {
    I18n::Message message, submessage;
    switch (j) {
      case k_indexOfTTest:
        message = isTestSubApp ? I18n::Message::TTest : I18n::Message::TInterval;
        submessage = I18n::Message::Recommended;
        break;
      case k_indexOfZTest:
        message = isTestSubApp ? I18n::Message::ZTest : I18n::Message::ZInterval;
        submessage = I18n::Message::RarelyUsed;
        break;
      case k_indexOfPooledTest:
        message = isTestSubApp ? I18n::Message::PooledTTest : I18n::Message::PooledTInterval;
        submessage = I18n::Message::RarelyUsed;
        break;
      default:
        assert(false);
    }
    Escher::MessageTableCellWithChevronAndMessage * mcell =
        static_cast<Escher::MessageTableCellWithChevronAndMessage *>(cell);
    mcell->setMessage(message);
    mcell->setSubtitle(submessage);
  }
}

int Probability::TypeController::indexFromListIndex(int i) const {
  // Offset if needed
  if (App::app()->test() == Data::Test::OneMean && i >= k_indexOfPooledTest) {
    return i + 1;
  }
  return i;
}

int Probability::TypeController::listIndexFromIndex(int i) const {
  if (App::app()->test() == Data::Test::OneMean && i >= k_indexOfPooledTest) {
    return i - 1;
  }
  return i;
}

I18n::Message Probability::TypeController::messageForTest(Data::SubApp subapp, Data::Test t) const {
  if (subapp == Data::SubApp::Tests) {
    return t == Data::Test::OneMean ? I18n::Message::OneMeanTestDescr
                                    : I18n::Message::TwoMeanTestDescr;
  }
  return t == Data::Test::OneMean ? I18n::Message::OneMeanIntervalDescr
                                  : I18n::Message::TwoMeanIntervalDescr;
}
