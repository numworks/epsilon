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
#include "probability/data.h"
#include "type_controller.h"

using namespace Probability;

TypeController::TypeController(StackViewController * parent,
                               HypothesisController * hypothesisController)
    : SelectableListViewPage(parent),
      m_hypothesisController(hypothesisController),
      m_contentView(&m_selectableTableView, &m_description) {
  m_cells[k_indexOfTTest].setMessage(I18n::Message::TTest);
  m_cells[k_indexOfTTest].setSubtitle(I18n::Message::Recommended);
  m_cells[k_indexOfPooledTest].setMessage(I18n::Message::PooledTTest);
  m_cells[k_indexOfPooledTest].setSubtitle(I18n::Message::RarelyUsed);
  m_cells[k_indexOfZTest].setMessage(I18n::Message::ZTest);
  m_cells[k_indexOfZTest].setSubtitle(I18n::Message::RarelyUsed);

  m_description.setMessage(I18n::Message::TypeDescr);
}

void TypeController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Type);
  selectRowAccordingToType(App::app()->snapshot()->data()->testType());
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

HighlightCell * TypeController::reusableCell(int i, int type) { return &m_cells[i]; }

bool TypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::ViewController * view;
    switch (selectedRow()) {
      case k_indexOfTTest:
      case k_indexOfPooledTest:
      case k_indexOfZTest:
        view = m_hypothesisController;
        break;
    }
    assert(view != nullptr);
    openPage(view, false);
    return true;
  }
  return false;
}

void TypeController::selectRowAccordingToType(Data::TestType t) {
  int row = -1;
  switch (t) {
    case Data::TestType::TTest:
      row = k_indexOfTTest;
      break;
    case Data::TestType::PooledTTest:
      row = k_indexOfPooledTest;
      break;
    case Data::TestType::ZTest:
      row = k_indexOfZTest;
      break;
  }
  assert(row >= 0);
  selectRow(row);
}

Escher::View * TypeView::subviewAtIndex(int i) {
  switch (i) {
    case 0:
      return m_list;
    case 1:
      return m_description;
  }
}

const char * TypeController::title() {
  // TODO replace with messages
  char * testOn = "Test on ";
  int offset = strlen(testOn);
  memcpy(m_titleBuffer, testOn, offset);
  char * txt;
  switch (App::app()->snapshot()->data()->test()) {
    case Data::Test::OneProp:
      txt = "one proportion";
      break;
    case Data::Test::OneMean:
      txt = "one mean";
      break;
    case Data::Test::TwoProps:
      txt = "two proportions";
      break;
    case Data::Test::TwoMeans:
      txt = "two means";
      break;
  }
  assert(offset + strlen(txt) < sizeof(m_titleBuffer));
  memcpy(m_titleBuffer + offset, txt, strlen(txt));
  return m_titleBuffer;
}
