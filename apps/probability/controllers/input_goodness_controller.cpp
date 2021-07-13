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
#include "probability/text_helpers.h"

using namespace Probability;

InputGoodnessController::InputGoodnessController(
    StackViewController * parent,
    ResultsController * resultsController,
    Statistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
    Page(parent),
    m_resultsController(resultsController),
    m_inputTableView(&m_contentView,
                     inputEventHandlerDelegate,
                     static_cast<Chi2Statistic *>(statistic),
                     this),
    m_contentView(this, this, &m_inputTableView, inputEventHandlerDelegate, this),
    m_statistic(static_cast<Chi2Statistic *>(statistic)) {
}

void InputGoodnessController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::InputGoodness);
  if (m_statistic->threshold() == -1) {
    m_statistic->initThreshold(App::app()->subapp());
    // TODO can definitely be factored out
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(m_statistic->threshold(), buffer, bufferSize);
    m_contentView.setSignificanceCellText(buffer);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputGoodnessController::buttonAction() {
  m_statistic->computeTest();
  openPage(m_resultsController);
}

const char * Probability::InputGoodnessController::title() {
  const char * category = App::app()->categoricalType() == Data::CategoricalType::Goodness
                        ? "goodness of fit"
                        : "Homogeneity/Independence";
  sprintf(m_titleBuffer, "X2-test: %s", category);
  return m_titleBuffer;
}

bool Probability::InputGoodnessController::textFieldShouldFinishEditing(TextField * textField,
                                                                        Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

bool Probability::InputGoodnessController::textFieldDidFinishEditing(TextField * textField,
                                                                     const char * text,
                                                                     Ion::Events::Event event) {
  // Parse significance level
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }
  m_statistic->setThreshold(p);
  // Reparse text
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  defaultParseFloat(p, buffer, bufferSize);
  textField->setText(buffer);
  return true;
}

// ScrollViewDelegate
void Probability::InputGoodnessController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  int row = m_inputTableView.selectedRow();
  int col = m_inputTableView.selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != row) {
    // Make m_contentView to cell
    KDRect cellFrame = KDRect(m_inputTableView.cumulatedWidthFromIndex(col),
                              m_inputTableView.cumulatedHeightFromIndex(row),
                              m_inputTableView.columnWidth(col),
                              m_inputTableView.rowHeight(row));

    m_contentView.scrollToContentRect(cellFrame);

    m_contentView.layoutSubviews(true);
  }
}
