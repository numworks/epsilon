#include "input_goodness_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/models/data.h"

using namespace Probability;

InputGoodnessDataSource::InputGoodnessDataSource(Responder * parent, SelectableTableView * tableView,
                                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                 TextFieldDelegate * delegate) {
  m_header[0].setMessage(I18n::Message::Observed);
  m_header[1].setMessage(I18n::Message::Expected);
  m_header[0].setEven(true);
  m_header[1].setEven(true);

  for (int i = 0; i < k_numberOfColumns * k_initialNumberOfRows; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, delegate);
    m_cells[i].setEven((i / 2) % 2 == 0);
  }
}

HighlightCell * InputGoodnessDataSource::reusableCell(int i, int type) {
  assert(i < reusableCellCount(0));
  if (i < 2) {
    return &m_header[i];
  }
  return &m_cells[i];
}

InputGoodnessController::InputGoodnessController(StackViewController * parent, ResultsController * resultsController,
                                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                 TextFieldDelegate * textFieldDelegate)
    : Page(parent),
      m_resultsController(resultsController),
      m_data(parent, &m_dataTable, inputEventHandlerDelegate, textFieldDelegate),
      m_contentView(this, this, &m_dataTable, inputEventHandlerDelegate, textFieldDelegate),
      m_dataTable(&m_contentView, &m_data, m_contentView.selectionDataSource()) {}

void InputGoodnessController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::InputGoodness);
  Escher::Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputGoodnessController::buttonAction() {
  openPage(m_resultsController);
}
