#ifndef SHARED_STORE_PARAM_CONTROLLER_H
#define SHARED_STORE_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/switch_view.h>

#include "column_parameter_controller.h"

namespace Shared {

class StoreController;

class StoreParameterController : public ColumnParameterController {
 public:
  StoreParameterController(Escher::Responder* parentResponder,
                           StoreColumnHelper* storeColumnHelper);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::AbstractMenuCell* cell(int index) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  void initializeColumnParameters() override;

 protected:
  constexpr static int k_numberOfCells = 4;

  StoreColumnHelper* m_storeColumnHelper;
  Escher::MenuCell<Escher::OneLineBufferTextView<KDFont::Size::Large>>
      m_clearColumn;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_hideCell;

 private:
  ColumnNameHelper* columnNameHelper() override;
  virtual I18n::Message sortMessage() { return I18n::Message::SortValues; }

  Escher::MenuCell<Escher::MessageTextView> m_fillFormula;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView> m_sortCell;
};

}  // namespace Shared

#endif
