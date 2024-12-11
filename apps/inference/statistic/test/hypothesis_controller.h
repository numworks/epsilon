#ifndef INFERENCE_STATISTIC_TEST_HYPOTHESIS_CONTROLLER_H
#define INFERENCE_STATISTIC_TEST_HYPOTHESIS_CONTROLLER_H

#include <escher/button_cell.h>
#include <escher/dropdown_widget.h>
#include <escher/highlight_cell.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/view.h>

#include "inference/statistic/comparison_operator_popup_data_source.h"
#include "inference/statistic/dataset_controller.h"
#include "inference/statistic/input_controller.h"
#include "inference/statistic/store/input_store_controller.h"

namespace Inference {

class HypothesisController
    : public Escher::ExplicitSelectableListViewController,
      public Escher::TextFieldDelegate,
      public Escher::DropdownCallback {
 public:
  HypothesisController(Escher::StackViewController* parent,
                       InputController* inputController,
                       InputStoreController* inputStoreController,
                       DatasetController* datasetController, Test* test);
  static bool ButtonAction(HypothesisController* controller, void* s);

  // SelectableListViewController
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  };
  const char* title() const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* cell(int i) override;
  int numberOfRows() const override { return 3; }
  KDCoordinate separatorBeforeRow(int row) override {
    return cell(row) == &m_next ? k_defaultRowSeparator : 0;
  }
  bool canStoreCellAtRow(int row) override { return false; }

  // TextFieldDelegate
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void textFieldDidAbortEditing(Escher::AbstractTextField* textField) override;
  bool textFieldIsEditable(Escher::AbstractTextField* textField) override {
    // AbstractWithEditableText shortcuts the delegates chain.
    assert(false);
    return true;
  }
  bool textFieldIsStorable(Escher::AbstractTextField* textField) override {
    return false;
  }

  // DropdownCallback
  void onDropdownSelected(int selectedRow) override;

 private:
  void loadHypothesisParam();
  const char* symbolPrefix();

  constexpr static int k_indexOfH0 = 0;
  constexpr static int k_indexOfHa = 1;
  constexpr static int k_cellBufferSize =
      7 /* μ1-μ2 */ + 1 /* = */ +
      Constants::k_shortFloatNumberOfChars /* float */ + 1 /* \0 */;
  InputController* m_inputController;
  InputStoreController* m_inputStoreController;
  DatasetController* m_datasetController;

  ComparisonOperatorPopupDataSource m_operatorDataSource;

  ParameterCell m_h0;
  Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                   Escher::DropdownWidget>
      m_ha;
  Escher::Dropdown m_haDropdown;
  Escher::ButtonCell m_next;

  constexpr static int k_titleBufferSize =
      Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];
  Test* m_test;
};

}  // namespace Inference

#endif
