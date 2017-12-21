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
  ctx->strokeRect(KDRect(k_margin, k_margin, ImageCell::k_width+2*k_outline, ImageCell::k_height+2*k_outline), Palette::GreyMiddle);
}

KDSize ResponderImageCell::minimalSizeForOptimalDisplay() const {
  return KDSize(2*k_totalMargin+ImageCell::k_width, 2*k_totalMargin*ImageCell::k_height);
}

bool ResponderImageCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Down) {
    KDPoint topLeftAngle = app()->modalView()->pointFromPointInView(this, KDPoint(k_totalMargin, k_totalMargin));
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
  m_imageCell.setFrame(KDRect(k_totalMargin, k_totalMargin, bounds().width()-2*k_totalMargin, bounds().height()-2*k_totalMargin));
}

}
