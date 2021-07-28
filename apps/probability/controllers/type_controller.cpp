#include "type_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include <string.h>

#include "categorical_type_controller.h"
#include "hypothesis_controller.h"
#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"
#include "type_controller.h"

using namespace Probability;

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
  m_description.setAlignment(0.5, 0);
  m_description.setFont(KDFont::SmallFont);
}

void TypeController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Type);
  m_description.setMessage(messageForTest(App::app()->test()));
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
    }
    if (App::app()->subapp() == Data::SubApp::Intervals) {
      view = m_inputController;
    } else {
      view = m_hypothesisController;
    }
    assert(view != nullptr);
    if (t != App::app()->testType() ||
        App::app()->categoricalType() != Data::CategoricalType::Unset) {
      App::app()->setCategoricalType(Data::CategoricalType::Unset);
      Statistic::initializeStatistic(m_statistic,
                                     App::app()->test(),
                                     t,
                                     Data::CategoricalType::Unset);
    }
    *m_globalTestType = t;
    openPage(view);
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
  // TODO replace with messages
  snprintf(m_titleBuffer, sizeof(m_titleBuffer), "Test on %s", testToText(App::app()->test()));
  return m_titleBuffer;
}

int Probability::TypeController::numberOfRows() const {
  return listIndexFromIndex(k_numberOfRows);
}

void Probability::TypeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  int j = indexFromListIndex(i);
  if (j < k_indexOfDescription) {
    I18n::Message message, submessage;
    switch (j) {
      case k_indexOfTTest:
        message = I18n::Message::TTest;
        submessage = I18n::Message::Recommended;
        break;
      case k_indexOfZTest:
        message = I18n::Message::ZTest;
        submessage = I18n::Message::RarelyUsed;
        break;
      case k_indexOfPooledTest:
        message = I18n::Message::PooledTTest;
        submessage = I18n::Message::RarelyUsed;
        break;
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

I18n::Message Probability::TypeController::messageForTest(Data::Test t) const {
  return t == Data::Test::OneMean ? I18n::Message::OneMeanTestDescr
                                  : I18n::Message::TwoMeanTestDescr;
}
