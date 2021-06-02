#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/button_with_separator.h>
#include <apps/shared/cell_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/page_controller.h"
#include "probability/models/data.h"
#include "results_controller.h"

namespace Probability {

class InputController : public FloatParameterPage {
public:
  InputController(Escher::StackViewController * parent, ResultsController * resultsController,
                  InputParameters * inputParameters, Escher::InputEventHandlerDelegate * handler);
  int numberOfRows() const override {
    return m_inputParameters->numberOfParameters() + 1 /* significance */ + 1 /* button */;
  }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  int typeAtIndex(int i) override;
  void didBecomeFirstResponder() override;
  void buttonAction() override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

protected:
  float parameterAtIndex(int i) override { return m_inputParameters->paramAtIndex(i); }

private:
  int reusableParameterCellCount(int type) override { return k_numberOfReusableCells; }
  Escher::HighlightCell * reusableParameterCell(int index, int type);
  bool setParameterAtIndex(int parameterIndex, float f) {
    m_inputParameters->setParamAtIndex(parameterIndex, f);
  }

  InputParameters * m_inputParameters;
  char m_titleBuffer[30];
  ResultsController * m_resultsController;

  constexpr static int k_numberOfReusableCells = 8;  // TODO count
  constexpr static int k_significanceCellType = 2;

  Escher::MessageTableCellWithEditableTextWithMessage m_parameterCells[k_numberOfReusableCells];
  Escher::MessageTableCellWithEditableTextWithMessage m_significanceCell;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H */
