#ifndef APPS_PROBABILITY_GUI_RESPONDER_IMAGE_CELL_H
#define APPS_PROBABILITY_GUI_RESPONDER_IMAGE_CELL_H

#include "probability/controllers/calculation_type_controller.h"
#include "probability/gui/highlight_image_cell.h"
#include "probability/gui/image_cell.h"
#include "probability/models/calculation/calculation.h"

namespace Probability {

class CalculationController;

class ResponderImageCell : public Escher::HighlightCell, public Escher::Responder {
public:
  ResponderImageCell(Escher::Responder * parentResponder,
                     Distribution * distribution,
                     Calculation * calculation,
                     CalculationController * calculationController);
  Escher::Responder * responder() override { return this; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setHighlighted(bool highlight) override;
  void setImage(const Escher::Image * image);
  constexpr static KDCoordinate k_outline = 1;
  constexpr static KDCoordinate k_oneCellWidth = 2 * k_outline + ImageCell::k_width;
  constexpr static KDCoordinate k_oneCellHeight = 2 * k_outline + ImageCell::k_height;

private:
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  HighlightImageCell m_imageCell;
  CalculationTypeController m_calculationTypeController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_RESPONDER_IMAGE_CELL_H */
