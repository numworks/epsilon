#ifndef OMG_DIRECTIONS_H
#define OMG_DIRECTIONS_H

#include <assert.h>

namespace OMG {

class Direction {
public:
  bool isLeft() const { return m_tag == Tag::Left; }
  bool isRight() const { return m_tag == Tag::Right; }
  bool isUp() const { return m_tag == Tag::Up; }
  bool isDown() const { return m_tag == Tag::Down; }

  bool isHorizontal() const { return m_tag == Tag::Left || m_tag == Tag::Right; }
  bool isVertical() const { return m_tag == Tag::Up || m_tag == Tag::Down; }

  operator class HorizontalDirection() const;
  operator class VerticalDirection() const;

  constexpr static Direction Left() { return Direction(Tag::Left); }
  constexpr static Direction Right() { return Direction(Tag::Right); }
  constexpr static Direction Up() { return Direction(Tag::Up); }
  constexpr static Direction Down() { return Direction(Tag::Down); }

  bool operator ==(const Direction &other) const { return m_tag == other.m_tag; }
  bool operator !=(const Direction &other) const { return m_tag != other.m_tag; }

protected:
  enum class Tag {
    Left,
    Right,
    Up,
    Down
  };

  constexpr Direction(Tag tag) : m_tag(tag) {}

private:
  Tag m_tag;
};

class HorizontalDirection : public Direction {
public:
  constexpr static HorizontalDirection Left() { return HorizontalDirection(Tag::Left); }
  constexpr static HorizontalDirection Right() { return HorizontalDirection(Tag::Right); }
private:
  constexpr HorizontalDirection(Tag tag) : Direction(tag) { assert(tag == Tag::Left || tag == Tag::Right); }
};

class VerticalDirection : public Direction {
public:
  constexpr static VerticalDirection Up() { return VerticalDirection(Tag::Up); }
  constexpr static VerticalDirection Down() { return VerticalDirection(Tag::Down); }
private:
  constexpr VerticalDirection(Tag tag) : Direction(tag) { assert(tag == Tag::Up || tag == Tag::Down); }
};

}

#endif
