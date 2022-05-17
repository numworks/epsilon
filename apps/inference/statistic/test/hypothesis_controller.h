#ifndef PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/palette.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view.h>

#include "inference/statistic/chi_square_and_slope/input_slope_controller.h"
#include "inference/statistic/comparison_operator_popup_data_source.h"
#include "inference/statistic/expression_cell_with_sublabel_and_dropdown.h"
#include "inference/statistic/input_controller.h"
#include "inference/shared/expression_cell_with_editable_text_with_message.h"
#include <escher/selectable_list_view_controller.h>

using namespace Escher;

namespace Inference {

class HypothesisController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>,
                             public Escher::TextFieldDelegate,
                             public Escher::DropdownCallback {
public:
  HypothesisController(Escher::StackViewController * parent,
                       InputController * inputController,
                       InputSlopeController * inputSlopeController,
                       InputEventHandlerDelegate * handler,
                       Test * test);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  };
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override { return 3; }
  static bool ButtonAction(void * c, void * s);

  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;

  // Escher::DropdownCallback
  void onDropdownSelected(int selectedRow) override;

private:
  void loadHypothesisParam();
  const char * symbolPrefix();

  constexpr static int k_indexOfH0 = 0;
  constexpr static int k_indexOfHa = 1;
  constexpr static int k_indexOfNext = 2;
  constexpr static int k_cellBufferSize = 7 /* μ1-μ2 */ + 1 /* = */ +
                                          Constants::k_shortFloatNumberOfChars /* float */ +
                                          1 /* \0 */;
  InputController * m_inputController;
  InputSlopeController * m_inputSlopeController;

  ComparisonOperatorPopupDataSource m_operatorDataSource;

  ExpressionCellWithEditableTextWithMessage m_h0;
  ExpressionCellWithSublabelAndDropdown m_ha;
  Shared::ButtonWithSeparator m_next;

  static constexpr int k_titleBufferSize = Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  char m_titleBuffer[k_titleBufferSize];
  Test * m_test;
};

}  // namespace Inference

#endif /* PROBABILITY_CONTROLLERS_HYPOTHESIS_CONTROLLER_H */
