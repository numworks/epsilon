#include <omg/directions.h>

namespace OMG {

Direction::Direction(Ion::Events::Event event) {
  assert(event.isMoveEvent() || event.isSelectionEvent());
  if (event == Ion::Events::Left || event == Ion::Events::ShiftLeft) {
    m_tag = Tag::Left;
  } else if (event == Ion::Events::Up || event == Ion::Events::ShiftUp) {
    m_tag = Tag::Up;
  } else if (event == Ion::Events::Down || event == Ion::Events::ShiftDown) {
    m_tag = Tag::Down;
  } else {
    assert(event == Ion::Events::Right || event == Ion::Events::ShiftRight);
    m_tag = Tag::Right;
  }
}

Direction::operator class HorizontalDirection() const {
  assert(isHorizontal()); return * static_cast<const HorizontalDirection *>(this);
}

Direction::operator class VerticalDirection() const {
  assert(isVertical()); return * static_cast<const VerticalDirection *>(this);
}

}
