#ifndef ION_EVENTS_H
#define ION_EVENTS_H

#include <ion/keyboard.h>

namespace Ion {
namespace Events {

class Event {
public:
  static constexpr Event PlainKey(Keyboard::Key k) { return Event((int)k); }
  static constexpr Event ShiftKey(Keyboard::Key k) { return Event(PageSize+(int)k); }
  static constexpr Event AlphaKey(Keyboard::Key k) { return Event(2*PageSize+(int)k); }
  static constexpr Event ShiftAlphaKey(Keyboard::Key k) { return Event(3*PageSize+(int)k); }
  static constexpr Event Special(int i) { return Event(4*PageSize+i); }

  constexpr Event() : m_id(4*PageSize){} // Return Ion::Event::None by default
  constexpr Event(int i) : m_id(i){} // TODO: Assert here that i>=0 && i<255
#if DEBUG
  uint8_t id() const { return m_id; }
  const char * name() const;
#endif
  Event(Keyboard::Key key, bool shift, bool alpha);

  bool operator==(const Event & other) const {
    return (m_id == other.m_id);
  }
  bool operator!=(const Event & other) const {
    return (m_id != other.m_id);
  }
  const char * text() const;
  bool hasText() const;
  bool isKeyboardEvent() const { return m_id < 4*PageSize; }
  bool isSpecialEvent() const { return m_id >= 4*PageSize; }
  bool isDefined() const;
  static constexpr int PageSize = Keyboard::NumberOfKeys;
private:
  uint8_t m_id;
};

enum class ShiftAlphaStatus {
  Default,
  Shift,
  Alpha,
  ShiftAlpha,
  AlphaLock,
  ShiftAlphaLock,
};

// Timeout is decremented
Event getEvent(int * timeout);

ShiftAlphaStatus shiftAlphaStatus();
void setShiftAlphaStatus(ShiftAlphaStatus s);
bool isShiftActive();
bool isAlphaActive();
void updateModifiersFromEvent(Event e);

// Plain

constexpr Event Left  = Event::PlainKey(Keyboard::Key::A1);
constexpr Event Up    = Event::PlainKey(Keyboard::Key::A2);
constexpr Event Down  = Event::PlainKey(Keyboard::Key::A3);
constexpr Event Right = Event::PlainKey(Keyboard::Key::A4);
constexpr Event OK    = Event::PlainKey(Keyboard::Key::A5);
constexpr Event Back  = Event::PlainKey(Keyboard::Key::A6);

constexpr Event Home  = Event::PlainKey(Keyboard::Key::B1);
constexpr Event OnOff = Event::PlainKey(Keyboard::Key::B2);

constexpr Event Shift = Event::PlainKey(Keyboard::Key::C1);
constexpr Event Alpha = Event::PlainKey(Keyboard::Key::C2);
constexpr Event XNT   = Event::PlainKey(Keyboard::Key::C3);
constexpr Event Var   = Event::PlainKey(Keyboard::Key::C4);
constexpr Event Toolbox = Event::PlainKey(Keyboard::Key::C5);
constexpr Event Backspace = Event::PlainKey(Keyboard::Key::C6);

constexpr Event Exp = Event::PlainKey(Keyboard::Key::D1);
constexpr Event Ln = Event::PlainKey(Keyboard::Key::D2);
constexpr Event Log = Event::PlainKey(Keyboard::Key::D3);
constexpr Event Imaginary = Event::PlainKey(Keyboard::Key::D4);
constexpr Event Comma = Event::PlainKey(Keyboard::Key::D5);
constexpr Event Power = Event::PlainKey(Keyboard::Key::D6);

constexpr Event Sine = Event::PlainKey(Keyboard::Key::E1);
constexpr Event Cosine = Event::PlainKey(Keyboard::Key::E2);
constexpr Event Tangent = Event::PlainKey(Keyboard::Key::E3);
constexpr Event Pi = Event::PlainKey(Keyboard::Key::E4);
constexpr Event Sqrt = Event::PlainKey(Keyboard::Key::E5);
constexpr Event Square = Event::PlainKey(Keyboard::Key::E6);

constexpr Event Seven = Event::PlainKey(Keyboard::Key::F1);
constexpr Event Eight = Event::PlainKey(Keyboard::Key::F2);
constexpr Event Nine = Event::PlainKey(Keyboard::Key::F3);
constexpr Event LeftParenthesis = Event::PlainKey(Keyboard::Key::F4);
constexpr Event RightParenthesis = Event::PlainKey(Keyboard::Key::F5);

constexpr Event Four = Event::PlainKey(Keyboard::Key::G1);
constexpr Event Five = Event::PlainKey(Keyboard::Key::G2);
constexpr Event Six = Event::PlainKey(Keyboard::Key::G3);
constexpr Event Multiplication = Event::PlainKey(Keyboard::Key::G4);
constexpr Event Division = Event::PlainKey(Keyboard::Key::G5);

constexpr Event One  = Event::PlainKey(Keyboard::Key::H1);
constexpr Event Two = Event::PlainKey(Keyboard::Key::H2);
constexpr Event Three = Event::PlainKey(Keyboard::Key::H3);
constexpr Event Plus = Event::PlainKey(Keyboard::Key::H4);
constexpr Event Minus = Event::PlainKey(Keyboard::Key::H5);

constexpr Event Zero = Event::PlainKey(Keyboard::Key::I1);
constexpr Event Dot = Event::PlainKey(Keyboard::Key::I2);
constexpr Event EE = Event::PlainKey(Keyboard::Key::I3);
constexpr Event Ans = Event::PlainKey(Keyboard::Key::I4);
constexpr Event EXE = Event::PlainKey(Keyboard::Key::I5);

// Shift

constexpr Event ShiftLeft  = Event::ShiftKey(Keyboard::Key::A1);
constexpr Event ShiftRight = Event::ShiftKey(Keyboard::Key::A4);

constexpr Event AlphaLock = Event::ShiftKey(Keyboard::Key::C2);
constexpr Event Cut = Event::ShiftKey(Keyboard::Key::C3);
constexpr Event Copy = Event::ShiftKey(Keyboard::Key::C4);
constexpr Event Paste = Event::ShiftKey(Keyboard::Key::C5);
constexpr Event Clear = Event::ShiftKey(Keyboard::Key::C6);

constexpr Event LeftBracket = Event::ShiftKey(Keyboard::Key::D1);
constexpr Event RightBracket = Event::ShiftKey(Keyboard::Key::D2);
constexpr Event LeftBrace = Event::ShiftKey(Keyboard::Key::D3);
constexpr Event RightBrace = Event::ShiftKey(Keyboard::Key::D4);
constexpr Event Underscore = Event::ShiftKey(Keyboard::Key::D5);
constexpr Event Sto = Event::ShiftKey(Keyboard::Key::D6);

constexpr Event Arcsine = Event::ShiftKey(Keyboard::Key::E1);
constexpr Event Arccosine = Event::ShiftKey(Keyboard::Key::E2);
constexpr Event Arctangent = Event::ShiftKey(Keyboard::Key::E3);
constexpr Event Equal = Event::ShiftKey(Keyboard::Key::E4);
constexpr Event Lower = Event::ShiftKey(Keyboard::Key::E5);
constexpr Event Greater = Event::ShiftKey(Keyboard::Key::E6);

// Alpha

constexpr Event Colon = Event::AlphaKey(Keyboard::Key::C3);
constexpr Event SemiColon = Event::AlphaKey(Keyboard::Key::C4);
constexpr Event DoubleQuotes = Event::AlphaKey(Keyboard::Key::C5);

constexpr Event LowerA = Event::AlphaKey(Keyboard::Key::D1);
constexpr Event LowerB = Event::AlphaKey(Keyboard::Key::D2);
constexpr Event LowerC = Event::AlphaKey(Keyboard::Key::D3);
constexpr Event LowerD = Event::AlphaKey(Keyboard::Key::D4);
constexpr Event LowerE = Event::AlphaKey(Keyboard::Key::D5);
constexpr Event LowerF = Event::AlphaKey(Keyboard::Key::D6);

constexpr Event LowerG = Event::AlphaKey(Keyboard::Key::E1);
constexpr Event LowerH = Event::AlphaKey(Keyboard::Key::E2);
constexpr Event LowerI = Event::AlphaKey(Keyboard::Key::E3);
constexpr Event LowerJ = Event::AlphaKey(Keyboard::Key::E4);
constexpr Event LowerK = Event::AlphaKey(Keyboard::Key::E5);
constexpr Event LowerL = Event::AlphaKey(Keyboard::Key::E6);

constexpr Event LowerM = Event::AlphaKey(Keyboard::Key::F1);
constexpr Event LowerN = Event::AlphaKey(Keyboard::Key::F2);
constexpr Event LowerO = Event::AlphaKey(Keyboard::Key::F3);
constexpr Event LowerP = Event::AlphaKey(Keyboard::Key::F4);
constexpr Event LowerQ = Event::AlphaKey(Keyboard::Key::F5);

constexpr Event LowerR = Event::AlphaKey(Keyboard::Key::G1);
constexpr Event LowerS = Event::AlphaKey(Keyboard::Key::G2);
constexpr Event LowerT = Event::AlphaKey(Keyboard::Key::G3);
constexpr Event LowerU = Event::AlphaKey(Keyboard::Key::G4);
constexpr Event LowerV = Event::AlphaKey(Keyboard::Key::G5);

constexpr Event LowerW = Event::AlphaKey(Keyboard::Key::H1);
constexpr Event LowerX = Event::AlphaKey(Keyboard::Key::H2);
constexpr Event LowerY = Event::AlphaKey(Keyboard::Key::H3);
constexpr Event LowerZ = Event::AlphaKey(Keyboard::Key::H4);
constexpr Event Space = Event::AlphaKey(Keyboard::Key::H5);

constexpr Event Question = Event::AlphaKey(Keyboard::Key::I1);
constexpr Event Exclamation = Event::AlphaKey(Keyboard::Key::I2);

// Shift + Alpha

constexpr Event UpperA = Event::ShiftAlphaKey(Keyboard::Key::D1);
constexpr Event UpperB = Event::ShiftAlphaKey(Keyboard::Key::D2);
constexpr Event UpperC = Event::ShiftAlphaKey(Keyboard::Key::D3);
constexpr Event UpperD = Event::ShiftAlphaKey(Keyboard::Key::D4);
constexpr Event UpperE = Event::ShiftAlphaKey(Keyboard::Key::D5);
constexpr Event UpperF = Event::ShiftAlphaKey(Keyboard::Key::D6);

constexpr Event UpperG = Event::ShiftAlphaKey(Keyboard::Key::E1);
constexpr Event UpperH = Event::ShiftAlphaKey(Keyboard::Key::E2);
constexpr Event UpperI = Event::ShiftAlphaKey(Keyboard::Key::E3);
constexpr Event UpperJ = Event::ShiftAlphaKey(Keyboard::Key::E4);
constexpr Event UpperK = Event::ShiftAlphaKey(Keyboard::Key::E5);
constexpr Event UpperL = Event::ShiftAlphaKey(Keyboard::Key::E6);

constexpr Event UpperM = Event::ShiftAlphaKey(Keyboard::Key::F1);
constexpr Event UpperN = Event::ShiftAlphaKey(Keyboard::Key::F2);
constexpr Event UpperO = Event::ShiftAlphaKey(Keyboard::Key::F3);
constexpr Event UpperP = Event::ShiftAlphaKey(Keyboard::Key::F4);
constexpr Event UpperQ = Event::ShiftAlphaKey(Keyboard::Key::F5);

constexpr Event UpperR = Event::ShiftAlphaKey(Keyboard::Key::G1);
constexpr Event UpperS = Event::ShiftAlphaKey(Keyboard::Key::G2);
constexpr Event UpperT = Event::ShiftAlphaKey(Keyboard::Key::G3);
constexpr Event UpperU = Event::ShiftAlphaKey(Keyboard::Key::G4);
constexpr Event UpperV = Event::ShiftAlphaKey(Keyboard::Key::G5);

constexpr Event UpperW = Event::ShiftAlphaKey(Keyboard::Key::H1);
constexpr Event UpperX = Event::ShiftAlphaKey(Keyboard::Key::H2);
constexpr Event UpperY = Event::ShiftAlphaKey(Keyboard::Key::H3);
constexpr Event UpperZ = Event::ShiftAlphaKey(Keyboard::Key::H4);

constexpr Event None = Event::Special(0);
constexpr Event Termination = Event::Special(1);
constexpr Event TimerFire = Event::Special(2);

}
}

#endif
