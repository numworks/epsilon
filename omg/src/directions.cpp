#include <omg/directions.h>

namespace OMG {

Direction::operator class HorizontalDirection() const {
  assert(isHorizontal()); return * static_cast<const HorizontalDirection *>(this);
}

Direction::operator class VerticalDirection() const {
  assert(isVertical()); return * static_cast<const VerticalDirection *>(this);
}

}
