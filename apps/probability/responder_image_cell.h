#ifndef PROBABILITY_RESPONDER_IMAGE_CELL_H
#define PROBABILITY_RESPONDER_IMAGE_CELL_H

#include <escher.h>
#include "calculation/calculation.h"
#include "calculation_type_controller.h"
#include "image_cell.h"

namespace Probability {

class ResponderImageCell : public HighlightCell, public Responder {
public:
  ResponderImageCell(Responder * parentResponder, Distribution * distribution, Calculation * calculation, CalculationController * calculationController);
  Responder * responder() override {
    return this;
  }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setHighlighted(bool highlight) override;
  void setImage(const Image * image, const Image * focusedImage);
  constexpr static KDCoordinate k_outline = 1;
  constexpr static KDCoordinate k_oneCellWidth = 2*k_outline+ImageCell::k_width;
  constexpr static KDCoordinate k_oneCellHeight = 2*k_outline+ImageCell::k_height;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ImageCell m_imageCell;
  CalculationTypeController m_calculationTypeController;
};

}

#endif
