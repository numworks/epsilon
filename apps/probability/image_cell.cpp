#include "image_cell.h"
#include <assert.h>

namespace Probability {

ImageCell::ImageCell() :
  HighlightCell(),
  m_icon(nullptr),
  m_focusedIcon(nullptr)
{
}

void ImageCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void ImageCell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
  setHighlighted(isHighlighted());
  markRectAsDirty(bounds());
}

int ImageCell::numberOfSubviews() const {
  return 1;
}

View * ImageCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_iconView;
}

void ImageCell::layoutSubviews(bool force) {
  m_iconView.setFrame(bounds(), force);
}

}
