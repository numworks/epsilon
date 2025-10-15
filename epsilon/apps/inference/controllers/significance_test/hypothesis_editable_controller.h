#pragma once

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

#include "hypothesis_controller.h"
#include "inference/controllers/comparison_operator_popup_data_source.h"
#include "inference/controllers/dataset_controller.h"
#include "inference/controllers/input_controller.h"
#include "inference/controllers/store/input_store_controller.h"

namespace Inference {

class HypothesisEditableController : public HypothesisController,
                                     public Escher::TextFieldDelegate,
                                     public Escher::DropdownCallback {
 public:
  HypothesisEditableController(Escher::StackViewController* parent,
                               InputController* inputController,
                               InputStoreController* inputStoreController,
                               DatasetController* datasetController,
                               SignificanceTest* test);

  // SelectableListViewController
  const Escher::HighlightCell* cell(int i) const override;

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

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  void loadHypothesisParam();
  const char* symbolPrefix() const;

  constexpr static int k_indexOfH0 = 0;
  constexpr static int k_indexOfHa = 1;
  constexpr static int k_cellBufferSize =
      7 /* μ1-μ2 */ + 1 /* = */ +
      Constants::k_shortFloatNumberOfChars /* float */ + 1 /* \0 */;

  ComparisonOperatorPopupDataSource m_operatorDataSource;

  ParameterCell m_h0;
  Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                   Escher::DropdownWidget>
      m_ha;
  Escher::Dropdown m_haDropdown;
};

}  // namespace Inference
