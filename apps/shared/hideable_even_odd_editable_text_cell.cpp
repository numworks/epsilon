#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

KDColor HideableEvenOddEditableTextCell::backgroundColor() const {
  if (hidden()) {
    return hideColor();
  }
  return EvenOddEditableTextCell::backgroundColor();
}

void HideableEvenOddEditableTextCell::setHide(bool hide) {
  if (hidden() != hide) {
    Hideable::setHide(hide);
    editableTextCell()->textField()->setBackgroundColor(backgroundColor());
    reloadCell();
  }
}

}
