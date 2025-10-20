#pragma once

#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <inference/models/significance_test.h>

namespace Inference {

class ControllerContainer;

class HypothesisController
    : public Escher::ExplicitSelectableListViewController {
 public:
  HypothesisController(Escher::StackViewController* parent,
                       ControllerContainer* controllerContainer,
                       SignificanceTest* test);

  static bool ButtonAction(HypothesisController* controller, void* s);

  // SelectableListViewController
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  };
  const char* title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 3; }
  KDCoordinate separatorBeforeRow(int row) const override {
    return cell(row) == &m_next ? k_defaultRowSeparator : 0;
  }
  bool canStoreCellAtRow(int row) override { return false; }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

  Escher::ButtonCell m_next;
  SignificanceTest* m_test;

 private:
  ControllerContainer* m_controllerContainer;

  constexpr static int k_titleBufferSize =
      Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];
};

}  // namespace Inference
