#include "hideable_even_odd_cell.h"

namespace Shared {

KDColor HideableEvenOddCell::backgroundColor() const {
  if (hidden()) {
    return hideColor();
  }
  return EvenOddCell::backgroundColor();
}

void HideableEvenOddCell::setHide(bool hide) {
  if (hidden() != hide) {
    Hideable::setHide(hide);
    reloadCell();
  }
}

}
