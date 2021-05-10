#include "input_goodness_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

InputGoodnessView::InputGoodnessView(Responder * parentResponder, TableViewDataSource * dataSource,
                                     InputEventHandlerDelegate * inputEventHandlerDelegate,
                                     TextFieldDelegate * textFieldDelegate)
    : VerticalLayout(),
      m_tableView(parentResponder, dataSource, this),
      m_significance(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
      m_next(parentResponder, I18n::Message::Ok,
             Escher::Invocation(
                 [](void * ctx, void * sender) -> bool {
                   reinterpret_cast<InputGoodnessView *>(ctx)->buttonAction();
                   return true;
                 },
                 this)) {
  // m_significance.setMessage(I18n::Message::A);
  // m_significance.setSubLabelMessage(I18n::Message::SignificanceLevel);
  // m_significance.setAccessoryText("0.05");
  setView(&m_tableView, 0);
  setView(&m_significance, 1);
  setView(&m_next, 2);
}

InputGoodnessDataSource::InputGoodnessDataSource() : TableViewDataSource(), m_observed(), m_expected(), m_cells() {
  m_cells[0].setLabelText("Observed");
  m_cells[1].setLabelText("Expected");
}

HighlightCell * InputGoodnessDataSource::reusableCell(int i, int type) {
  assert(i < reusableCellCount(0));
  return &m_cells[i];
}

InputGoodnessController::InputGoodnessController(Responder * parent,
                                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                 TextFieldDelegate * textFieldDelegate)
    : ViewController(parent), m_contentView(this, this, inputEventHandlerDelegate, textFieldDelegate) {}