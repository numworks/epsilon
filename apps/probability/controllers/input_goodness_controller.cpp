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

using namespace Probability;

InputGoodnessView::InputGoodnessView(Responder * parentResponder, TableViewDataSource * dataSource,
                                     InputEventHandlerDelegate * inputEventHandlerDelegate,
                                     TextFieldDelegate * textFieldDelegate)
    : VerticalLayout(),
      Responder(parentResponder),
      m_tableView(this, dataSource, &m_tableSelection),
      m_significance(this, inputEventHandlerDelegate, textFieldDelegate),
      m_next(this, I18n::Message::Ok, buttonActionInvocation()) {
  // m_significance.setMessage(I18n::Message::A);
  // m_significance.setSubLabelMessage(I18n::Message::SignificanceLevel);
  // m_significance.setAccessoryText("0.05");
  setView(&m_tableView, 0);
  setView(&m_significance, 1);
  setView(&m_next, 2);
}

void InputGoodnessView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(2);
  }
  setResponderForSelectedRow();
  highlightViewForSelectedRow();
}

bool InputGoodnessView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - 1);
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < k_numberOfChildren - 1) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + 1);
    }
    setResponderForSelectedRow();
    highlightViewForSelectedRow();
    return true;
  }
  return false;
}

Responder * InputGoodnessView::responderForRow(int row) {
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      return &m_tableView;
    case k_indexOfSignificance:
      return &m_significance;
    case k_indexOfNext:
      return &m_next;
  }
  assert(false);
}

void InputGoodnessView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

void Probability::InputGoodnessView::highlightViewForSelectedRow() {
  // TODO set behavior in didBecomeFirstResponder?
  m_significance.setHighlighted(false);
  m_next.setHighlighted(false);
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      if (m_tableView.selectedRow() < 0) {
        m_tableView.selectCellAtLocation(0, 0);  // TODO or last ?
      }
      break;
    case k_indexOfSignificance:
      m_significance.setHighlighted(true);
      break;
    case k_indexOfNext:
      m_next.setHighlighted(true);
      break;
  }
}

void Probability::InputGoodnessDataSource::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  auto evenCell = static_cast<EvenOddEditableTextCell *>(cell);
  evenCell->setEven(j % 2 == 0);
}

InputGoodnessDataSource::InputGoodnessDataSource(Responder * parent, SelectableTableView * tableView,
                                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                 TextFieldDelegate * delegate)
    : TableViewDataSource(), m_observed(), m_expected(), m_cells() {
  m_header[0].setMessage(I18n::Message::Observed);
  m_header[1].setMessage(I18n::Message::Expected);
  for (int i = 0; i < 10; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, delegate);
  }
}

HighlightCell * InputGoodnessDataSource::reusableCell(int i, int type) {
  assert(i < reusableCellCount(0));
  if (i < 2) {
    return &m_header[i];
  }
  return &m_cells[i];
}

InputGoodnessController::InputGoodnessController(StackViewController * parent,
                                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                 TextFieldDelegate * textFieldDelegate)
    : Page(parent),
      InputGoodnessDataSource(parent, m_contentView.selectableTableView(), inputEventHandlerDelegate,
                              textFieldDelegate),
      m_contentView(this, this, inputEventHandlerDelegate, textFieldDelegate) {}

void InputGoodnessController::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(&m_contentView);
}
