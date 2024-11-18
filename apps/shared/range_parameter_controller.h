#ifndef SHARED_RANGE_PARAMETER_CONTROLLER_H
#define SHARED_RANGE_PARAMETER_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/transparent_image_view.h>
#include <escher/unequal_view.h>

#include "grid_type_controller.h"
#include "interactive_curve_view_range.h"
#include "pop_up_controller.h"
#include "single_interactive_curve_view_range_controller.h"

namespace Shared {

class RangeParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  using MenuCell =
      Escher::MenuCell<Escher::MessageTextView, Escher::OneLineBufferTextView<>,
                       Escher::ChevronView>;
  RangeParameterController(
      Escher::Responder* parentResponder,
      InteractiveCurveViewRange* interactiveCurveViewRange);

  const char* title() override { return I18n::translate(I18n::Message::Axis); }

  int numberOfRows() const override { return 5; }
  Escher::HighlightCell* cell(int row) override;
  KDCoordinate separatorBeforeRow(int row) override;

  void showGridTypeMenu(bool visible) { m_gridTypeCell.setVisible(visible); }

  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;

  void setRange(InteractiveCurveViewRange* range);
  Escher::StackViewController* stackController() {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }

 private:
  void buttonAction();
  void fillCells();

  InteractiveCurveViewRange* m_interactiveRange;
  InteractiveCurveViewRange m_tempInteractiveRange;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::UnequalView>
      m_normalizeCell;
  MenuCell m_xRangeCell;
  MenuCell m_yRangeCell;
  MenuCell m_gridTypeCell;
  Escher::ButtonCell m_okButton;
  Shared::MessagePopUpController m_confirmPopUpController;
  SingleInteractiveCurveViewRangeController
      m_singleInteractiveCurveViewRangeController;

  GridTypeController m_gridTypeController;
};

}  // namespace Shared

#endif
