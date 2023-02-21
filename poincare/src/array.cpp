#include <poincare/array.h>

namespace Poincare {

void Array::didChangeNumberOfChildren(int newNumberOfChildren) {
  if (newNumberOfChildren == 0) {
    m_numberOfRows = 0;
    m_numberOfColumns = 0;
    return;
  }
  m_numberOfRows = 1;
  m_numberOfColumns = newNumberOfChildren;
}

}  // namespace Poincare
