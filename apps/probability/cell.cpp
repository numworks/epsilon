#include "cell.h"
#include <assert.h>

using namespace Escher;

namespace Probability {

Cell::Cell() :
  TableCell(),
  m_labelView(KDFont::LargeFont, (I18n::Message)0, 0, 0.5, KDColorBlack, KDColorWhite),
  m_icon(nullptr),
  m_focusedIcon(nullptr)
{
}

void Cell::reloadCell() {
  HighlightCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void Cell::setLabel(I18n::Message message) {
  m_labelView.setMessage(message);
}

void Cell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
}

}
