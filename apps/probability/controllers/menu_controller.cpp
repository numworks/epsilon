#include "menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>

#include <new>

#include "../images/confidence_interval.h"
#include "../images/probability.h"
#include "../images/significance_test.h"
#include "probability/app.h"

using namespace Probability;

MenuController::MenuController(Escher::StackViewController * parentResponder,
                               DistributionController * distributionController,
                               TestController * testController,
                               Inference * inference) :
      Escher::SelectableCellListPage<Escher::SubappCell, k_numberOfCells>(parentResponder),
      m_distributionController(distributionController),
      m_testController(testController),
      m_inference(inference),
      m_contentView(&m_selectableTableView) {
  selectRow(0);
  cellAtIndex(k_indexOfProbability)->setMessages(I18n::Message::ProbaApp, I18n::Message::ProbaDescr);
  cellAtIndex(k_indexOfProbability)->setImage(ImageStore::Probability);
  cellAtIndex(k_indexOfTest)->setImage(ImageStore::SignificanceTest);
  cellAtIndex(k_indexOfTest)->setMessages(I18n::Message::Tests, I18n::Message::TestDescr);
  cellAtIndex(k_indexOfInterval)->setMessages(I18n::Message::Intervals, I18n::Message::IntervalDescr);
  cellAtIndex(k_indexOfInterval)->setImage(ImageStore::ConfidenceInterval);
}

void MenuController::stackOpenPage(Escher::ViewController * nextPage) {
  selectRow(static_cast<int>(m_inference->subApp()));
  ViewController::stackOpenPage(nextPage);
}

void MenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData(true);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Inference::SubApp subapp;
    Escher::SelectableListViewController * controller = nullptr;
    switch (selectedRow()) {
      case k_indexOfProbability:
        subapp = Inference::SubApp::Probability;
        controller = m_distributionController;
        break;
      case k_indexOfTest:
        subapp = Inference::SubApp::Test;
        controller = m_testController;
        break;
      case k_indexOfInterval:
        subapp = Inference::SubApp::Interval;
        controller = m_testController;
        break;
    }
    assert(controller != nullptr);
    Inference::Initialize(m_inference, subapp);
    stackOpenPage(controller);
    return true;
  }
  return false;
}
