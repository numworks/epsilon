#include "test_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "../gui/selectable_cell_list_controller.h"
#include "categorical_type_controller.h"
#include "hypothesis_controller.h"
#include "type_controller.h"

TestController::TestController(Escher::StackViewController * parentResponder, HypothesisController * hypothesisController,
                               TypeController * typeController, CategoricalTypeController * categoricalController)
    : SelectableCellListPage(parentResponder),
      m_hypothesisController(hypothesisController),
      m_categoricalController(categoricalController),
      m_typeController(typeController) {
  // Create cells
  m_cells[k_indexOfOneProp].setMessage(I18n::Message::TestOneProp);
  m_cells[k_indexOfOneProp].setSubtitle(I18n::Message::ZTest);
  m_cells[k_indexOfOneMean].setMessage(I18n::Message::TestOneMean);
  m_cells[k_indexOfOneMean].setSubtitle(I18n::Message::TOrZTest);
  m_cells[k_indexOfTwoProps].setMessage(I18n::Message::TestTwoProps);
  m_cells[k_indexOfTwoProps].setSubtitle(I18n::Message::ZTest);
  m_cells[k_indexOfTwoMeans].setMessage(I18n::Message::TestTwoMeans);
  m_cells[k_indexOfTwoMeans].setSubtitle(I18n::Message::TOrZTest);
  m_cells[k_indexOfCategorical].setMessage(I18n::Message::TestCategorical);
  m_cells[k_indexOfCategorical].setSubtitle(I18n::Message::X2Test);
}

void TestController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::ViewController * view;
    switch (selectedRow()) {
      case k_indexOfOneProp:
      case k_indexOfTwoProps:
        view = m_hypothesisController;
        break;
      case k_indexOfOneMean:
      case k_indexOfTwoMeans:
        view = m_typeController;
        break;
      case k_indexOfCategorical:
        view = m_categoricalController;
        break;
    }
    assert(view != nullptr);
    Escher::StackViewController * stack = (Escher::StackViewController *)parentResponder();
    stack->pop();  // Pop self
    stack->push(view);
    return true;
  }
  return false;
}
