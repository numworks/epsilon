#ifndef APPS_PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <apps/shared/text_field_delegate_app.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/layout_field.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view.h>

#include "input_controller.h"
#include "probability/abstract/button_delegate.h"
#include "probability/gui/comparison_operator_popup_data_source.h"
#include "probability/gui/layout_cell_with_editable_text_with_message.h"
#include "probability/gui/message_table_cell_with_sublabel_and_dropdown.h"
#include "probability/gui/page_controller.h"

using namespace Escher;

namespace Probability {

class NormalInputController;

class HypothesisController : public SelectableListViewPage,
                             public ButtonDelegate,
                             public Escher::TextFieldDelegate,
                             public DropdownCallback {
public:
  HypothesisController(Escher::StackViewController * parent,
                       InputController * inputController,
                       InputEventHandlerDelegate * handler,
                       Statistic * statistic);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  };
  const char * title() override;
  HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return 3; }
  void didBecomeFirstResponder() override;
  bool buttonAction() override;

  void openPage(ViewController * nextPage,
                KDColor backgroundColor = Escher::Palette::GrayDark,
                KDColor separatorColor = Escher::Palette::GrayDark,
                KDColor textColor = KDColorWhite) {
    SelectableListViewPage::openPage(nextPage, backgroundColor, separatorColor, textColor);
  }

  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textSizeDidChange) override;

  // DropdownCallback
  void onDropdownSelected(int selectedRow) override;

private:
  void loadHypothesisParam();
  const char * symbolPrefix();

  constexpr static int k_indexOfH0 = 0;
  constexpr static int k_indexOfHa = 1;
  constexpr static int k_indexOfNext = 2;
  InputController * m_inputController;

  ComparisonOperatorPopupDataSource m_operatorDataSource;

  LayoutCellWithEditableTextWithMessage m_h0;
  LayoutCellWithSublabelAndDropdown m_ha;
  Shared::ButtonWithSeparator m_next;

  char m_titleBuffer[40];
  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H */
