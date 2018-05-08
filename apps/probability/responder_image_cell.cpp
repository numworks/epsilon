#include "responder_image_cell.h"
#include <assert.h>

namespace Probability {

ResponderImageCell::ResponderImageCell(Responder * parentResponder, Law * law, Calculation * calculation, CalculationController * calculationController) :
  HighlightCell(),
  Responder(parentResponder),
  m_calculationTypeController(nullptr, law, calculation, calculationController)
{
}

void ResponderImageCell::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  ctx->strokeRect(KDRect(0, 0, ImageCell::k_width+2*k_outline, ImageCell::k_height+2*k_outline), Palette::GreyMiddle);
}

KDSize ResponderImageCell::minimalSizeForOptimalDisplay() const {
  return KDSize(2*k_outline+ImageCell::k_width, 2*k_outline*ImageCell::k_height);
}

bool ResponderImageCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Down) {
    KDPoint topLeftAngle = app()->modalView()->pointFromPointInView(this, KDPoint(k_outline, k_outline));
    app()->displayModalViewController(&m_calculationTypeController, 0.0f, 0.0f, topLeftAngle.y(), topLeftAngle.x());
    return true;
  }
  return false;
}

void ResponderImageCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_imageCell.setHighlighted(highlight);
}

void ResponderImageCell::setImage(const Image * image, const Image * focusedImage) {
  m_imageCell.setImage(image, focusedImage);
}

int ResponderImageCell::numberOfSubviews() const {
  return 1;
}

View * ResponderImageCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_imageCell;
}

void ResponderImageCell::layoutSubviews() {
  m_imageCell.setFrame(KDRect(k_outline, k_outline, bounds().width()-2*k_outline, bounds().height()-2*k_outline));
}

}
