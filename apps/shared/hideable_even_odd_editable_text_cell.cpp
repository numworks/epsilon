#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

KDColor HideableEvenOddEditableTextCell::backgroundColor() const {
  if (m_hide) {
    return hideColor();
  }
  return EvenOddEditableTextCell::backgroundColor();
}

void HideableEvenOddEditableTextCell::setHide(bool hide) {
  if (m_hide != hide) {
    m_hide = hide;
    editableTextCell()->textField()->setBackgroundColor(backgroundColor());
    reloadCell();
  }
}

}
