#pragma once

#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "inference/controllers/dataset_controller.h"
#include "inference/controllers/input_controller.h"
#include "inference/controllers/store/input_store_controller.h"

namespace Inference {

/* This controller is similar to HypothesisEditableController, but the
 * hypothesis data is only shown as a list, the fields are not editable */
class HypothesisDisplayOnlyController
    : public Escher::ExplicitSelectableListViewController {
 public:
  HypothesisDisplayOnlyController(Escher::StackViewController* parent,
                                  InputController* inputController,
                                  InputStoreController* inputStoreController,
                                  DatasetController* datasetController,
                                  SignificanceTest* test);

  static bool ButtonAction(HypothesisDisplayOnlyController* controller,
                           void* s);

  // SelectableListViewController
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  };
  const char* title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  const Escher::HighlightCell* cell(int i) const override;
  int numberOfRows() const override { return 3; }
  KDCoordinate separatorBeforeRow(int row) const override {
    return cell(row) == &m_next ? k_defaultRowSeparator : 0;
  }
  bool canStoreCellAtRow(int row) override { return false; }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  InputController* m_inputController;
  InputStoreController* m_inputStoreController;
  DatasetController* m_datasetController;

  using CellType = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::BufferTextView<10>>;
  CellType m_h0;
  CellType m_ha;
  Escher::ButtonCell m_next;

  constexpr static int k_titleBufferSize =
      Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];
  SignificanceTest* m_test;
};

}  // namespace Inference
