#ifndef OMG_DIRECTIONS_H
#define OMG_DIRECTIONS_H

#include <assert.h>
#include <ion/events.h>

namespace OMG {

class NewDirection {
 public:
  NewDirection(Ion::Events::Event event);

  bool isLeft() const { return m_tag == Tag::Left; }
  bool isRight() const { return m_tag == Tag::Right; }
  bool isUp() const { return m_tag == Tag::Up; }
  bool isDown() const { return m_tag == Tag::Down; }

  bool isHorizontal() const {
    return m_tag == Tag::Left || m_tag == Tag::Right;
  }
  bool isVertical() const { return m_tag == Tag::Up || m_tag == Tag::Down; }

  operator class NewHorizontalDirection() const;
  operator class NewVerticalDirection() const;

  constexpr static NewDirection Left() { return NewDirection(Tag::Left); }
  constexpr static NewDirection Right() { return NewDirection(Tag::Right); }
  constexpr static NewDirection Up() { return NewDirection(Tag::Up); }
  constexpr static NewDirection Down() { return NewDirection(Tag::Down); }

  bool operator==(const NewDirection &other) const { return m_tag == other.m_tag; }
  bool operator!=(const NewDirection &other) const { return m_tag != other.m_tag; }

 private:
  enum class Tag { Left, Right, Up, Down };

  constexpr NewDirection(Tag tag) : m_tag(tag) {}

  Tag m_tag;
};

class NewHorizontalDirection : public NewDirection {
 private:
  using NewDirection::Down;
  using NewDirection::Up;
};

class NewVerticalDirection : public NewDirection {
 private:
  using NewDirection::Left;
  using NewDirection::Right;
};

}  // namespace OMG

#endif
