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

  constexpr explicit operator uint8_t() const { return m_id; }
#ifndef NDEBUG
  const char * name() const;
#endif
  Event(Keyboard::Key key, bool shift, bool alpha, bool lock);

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
  const char * defaultText() const;

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

#if ION_EVENTS_JOURNAL
class Journal {
public:
  virtual void pushEvent(Event e) = 0;
  virtual Event popEvent() = 0;
  virtual bool isEmpty() = 0;
};

void replayFrom(Journal * l);
void logTo(Journal * l);
#endif

ShiftAlphaStatus shiftAlphaStatus();
void setShiftAlphaStatus(ShiftAlphaStatus s);
void removeShift();
bool isShiftActive();
bool isAlphaActive();
bool isLockActive();
void setLongRepetition(int longRepetition);
int repetitionFactor();
void updateModifiersFromEvent(Event e);
void didPressNewKey();

// Plain

constexpr Event Left  = Event::PlainKey(Keyboard::Key::Left);
constexpr Event Up    = Event::PlainKey(Keyboard::Key::Up);
constexpr Event Down  = Event::PlainKey(Keyboard::Key::Down);
constexpr Event Right = Event::PlainKey(Keyboard::Key::Right);
constexpr Event OK    = Event::PlainKey(Keyboard::Key::OK);
constexpr Event Back  = Event::PlainKey(Keyboard::Key::Back);

constexpr Event Home  = Event::PlainKey(Keyboard::Key::Home);
constexpr Event OnOff = Event::PlainKey(Keyboard::Key::OnOff);

constexpr Event Shift = Event::PlainKey(Keyboard::Key::Shift);
constexpr Event Alpha = Event::PlainKey(Keyboard::Key::Alpha);
constexpr Event XNT   = Event::PlainKey(Keyboard::Key::XNT);
constexpr Event Var   = Event::PlainKey(Keyboard::Key::Var);
constexpr Event Toolbox = Event::PlainKey(Keyboard::Key::Toolbox);
constexpr Event Backspace = Event::PlainKey(Keyboard::Key::Backspace);

constexpr Event Exp = Event::PlainKey(Keyboard::Key::Exp);
constexpr Event Ln = Event::PlainKey(Keyboard::Key::Ln);
constexpr Event Log = Event::PlainKey(Keyboard::Key::Log);
constexpr Event Imaginary = Event::PlainKey(Keyboard::Key::Imaginary);
constexpr Event Comma = Event::PlainKey(Keyboard::Key::Comma);
constexpr Event Power = Event::PlainKey(Keyboard::Key::Power);

constexpr Event Sine = Event::PlainKey(Keyboard::Key::Sine);
constexpr Event Cosine = Event::PlainKey(Keyboard::Key::Cosine);
constexpr Event Tangent = Event::PlainKey(Keyboard::Key::Tangent);
constexpr Event Pi = Event::PlainKey(Keyboard::Key::Pi);
constexpr Event Sqrt = Event::PlainKey(Keyboard::Key::Sqrt);
constexpr Event Square = Event::PlainKey(Keyboard::Key::Square);

constexpr Event Seven = Event::PlainKey(Keyboard::Key::Seven);
constexpr Event Eight = Event::PlainKey(Keyboard::Key::Eight);
constexpr Event Nine = Event::PlainKey(Keyboard::Key::Nine);
constexpr Event LeftParenthesis = Event::PlainKey(Keyboard::Key::LeftParenthesis);
constexpr Event RightParenthesis = Event::PlainKey(Keyboard::Key::RightParenthesis);

constexpr Event Four = Event::PlainKey(Keyboard::Key::Four);
constexpr Event Five = Event::PlainKey(Keyboard::Key::Five);
constexpr Event Six = Event::PlainKey(Keyboard::Key::Six);
constexpr Event Multiplication = Event::PlainKey(Keyboard::Key::Multiplication);
constexpr Event Division = Event::PlainKey(Keyboard::Key::Division);

constexpr Event One  = Event::PlainKey(Keyboard::Key::One);
constexpr Event Two = Event::PlainKey(Keyboard::Key::Two);
constexpr Event Three = Event::PlainKey(Keyboard::Key::Three);
constexpr Event Plus = Event::PlainKey(Keyboard::Key::Plus);
constexpr Event Minus = Event::PlainKey(Keyboard::Key::Minus);

constexpr Event Zero = Event::PlainKey(Keyboard::Key::Zero);
constexpr Event Dot = Event::PlainKey(Keyboard::Key::Dot);
constexpr Event EE = Event::PlainKey(Keyboard::Key::EE);
constexpr Event Ans = Event::PlainKey(Keyboard::Key::Ans);
constexpr Event EXE = Event::PlainKey(Keyboard::Key::EXE);

// Shift

constexpr Event ShiftHome  = Event::ShiftKey(Keyboard::Key::Home);

constexpr Event ShiftLeft  = Event::ShiftKey(Keyboard::Key::Left);
constexpr Event ShiftRight = Event::ShiftKey(Keyboard::Key::Right);
constexpr Event ShiftUp    = Event::ShiftKey(Keyboard::Key::Up);
constexpr Event ShiftDown  = Event::ShiftKey(Keyboard::Key::Down);
constexpr Event ShiftOK    = Event::ShiftKey(Keyboard::Key::OK);
constexpr Event ShiftBack  = Event::ShiftKey(Keyboard::Key::Back);

constexpr Event AlphaLock = Event::ShiftKey(Keyboard::Key::Alpha);
constexpr Event Cut = Event::ShiftKey(Keyboard::Key::XNT);
constexpr Event Copy = Event::ShiftKey(Keyboard::Key::Var);
constexpr Event Paste = Event::ShiftKey(Keyboard::Key::Toolbox);
constexpr Event Clear = Event::ShiftKey(Keyboard::Key::Backspace);

constexpr Event LeftBracket = Event::ShiftKey(Keyboard::Key::Exp);
constexpr Event RightBracket = Event::ShiftKey(Keyboard::Key::Ln);
constexpr Event LeftBrace = Event::ShiftKey(Keyboard::Key::Log);
constexpr Event RightBrace = Event::ShiftKey(Keyboard::Key::Imaginary);
constexpr Event Underscore = Event::ShiftKey(Keyboard::Key::Comma);
constexpr Event Sto = Event::ShiftKey(Keyboard::Key::Power);

constexpr Event Arcsine = Event::ShiftKey(Keyboard::Key::Sine);
constexpr Event Arccosine = Event::ShiftKey(Keyboard::Key::Cosine);
constexpr Event Arctangent = Event::ShiftKey(Keyboard::Key::Tangent);
constexpr Event Equal = Event::ShiftKey(Keyboard::Key::Pi);
constexpr Event Lower = Event::ShiftKey(Keyboard::Key::Sqrt);
constexpr Event Greater = Event::ShiftKey(Keyboard::Key::Square);

constexpr Event BrightnessPlus = Event::ShiftKey(Keyboard::Key::Plus);
constexpr Event BrightnessMinus = Event::ShiftKey(Keyboard::Key::Minus);

constexpr Event DoubleParenthesis = Event::ShiftKey(Keyboard::Key::LeftParenthesis);

constexpr Event ShiftZero = Event::ShiftKey(Keyboard::Key::Zero);
constexpr Event ShiftDot = Event::ShiftKey(Keyboard::Key::Dot);
constexpr Event ShiftEE = Event::ShiftKey(Keyboard::Key::EE);

constexpr Event ShiftOne = Event::ShiftKey(Keyboard::Key::One);
constexpr Event ShiftTwo = Event::ShiftKey(Keyboard::Key::Two);
constexpr Event ShiftThree = Event::ShiftKey(Keyboard::Key::Three);

constexpr Event ShiftFour = Event::ShiftKey(Keyboard::Key::Four);
constexpr Event ShiftFive = Event::ShiftKey(Keyboard::Key::Five);
constexpr Event ShiftSix = Event::ShiftKey(Keyboard::Key::Six);

constexpr Event ShiftSeven = Event::ShiftKey(Keyboard::Key::Seven);
constexpr Event ShiftEight = Event::ShiftKey(Keyboard::Key::Eight);
constexpr Event ShiftNine = Event::ShiftKey(Keyboard::Key::Nine);

constexpr Event ShiftEXE = Event::ShiftKey(Keyboard::Key::EXE);

// Alpha

constexpr Event AlphaLeft  = Event::AlphaKey(Keyboard::Key::Left);
constexpr Event AlphaRight = Event::AlphaKey(Keyboard::Key::Right);
constexpr Event AlphaUp    = Event::AlphaKey(Keyboard::Key::Up);
constexpr Event AlphaDown  = Event::AlphaKey(Keyboard::Key::Down);

constexpr Event Colon = Event::AlphaKey(Keyboard::Key::XNT);
constexpr Event SemiColon = Event::AlphaKey(Keyboard::Key::Var);
constexpr Event DoubleQuotes = Event::AlphaKey(Keyboard::Key::Toolbox);
constexpr Event Percent = Event::AlphaKey(Keyboard::Key::Backspace);

constexpr Event LowerA = Event::AlphaKey(Keyboard::Key::Exp);
constexpr Event LowerB = Event::AlphaKey(Keyboard::Key::Ln);
constexpr Event LowerC = Event::AlphaKey(Keyboard::Key::Log);
constexpr Event LowerD = Event::AlphaKey(Keyboard::Key::Imaginary);
constexpr Event LowerE = Event::AlphaKey(Keyboard::Key::Comma);
constexpr Event LowerF = Event::AlphaKey(Keyboard::Key::Power);

constexpr Event LowerG = Event::AlphaKey(Keyboard::Key::Sine);
constexpr Event LowerH = Event::AlphaKey(Keyboard::Key::Cosine);
constexpr Event LowerI = Event::AlphaKey(Keyboard::Key::Tangent);
constexpr Event LowerJ = Event::AlphaKey(Keyboard::Key::Pi);
constexpr Event LowerK = Event::AlphaKey(Keyboard::Key::Sqrt);
constexpr Event LowerL = Event::AlphaKey(Keyboard::Key::Square);

constexpr Event LowerM = Event::AlphaKey(Keyboard::Key::Seven);
constexpr Event LowerN = Event::AlphaKey(Keyboard::Key::Eight);
constexpr Event LowerO = Event::AlphaKey(Keyboard::Key::Nine);
constexpr Event LowerP = Event::AlphaKey(Keyboard::Key::LeftParenthesis);
constexpr Event LowerQ = Event::AlphaKey(Keyboard::Key::RightParenthesis);

constexpr Event LowerR = Event::AlphaKey(Keyboard::Key::Four);
constexpr Event LowerS = Event::AlphaKey(Keyboard::Key::Five);
constexpr Event LowerT = Event::AlphaKey(Keyboard::Key::Six);
constexpr Event LowerU = Event::AlphaKey(Keyboard::Key::Multiplication);
constexpr Event LowerV = Event::AlphaKey(Keyboard::Key::Division);

constexpr Event LowerW = Event::AlphaKey(Keyboard::Key::One);
constexpr Event LowerX = Event::AlphaKey(Keyboard::Key::Two);
constexpr Event LowerY = Event::AlphaKey(Keyboard::Key::Three);
constexpr Event LowerZ = Event::AlphaKey(Keyboard::Key::Plus);
constexpr Event Space = Event::AlphaKey(Keyboard::Key::Minus);

constexpr Event Question = Event::AlphaKey(Keyboard::Key::Zero);
constexpr Event Exclamation = Event::AlphaKey(Keyboard::Key::Dot);
constexpr Event alphaans = Event::AlphaKey(Keyboard::Key::Ans);

// Shift + Alpha
constexpr Event SimpleQuote = Event::ShiftAlphaKey(Keyboard::Key::Toolbox);

constexpr Event UpperA = Event::ShiftAlphaKey(Keyboard::Key::Exp);
constexpr Event UpperB = Event::ShiftAlphaKey(Keyboard::Key::Ln);
constexpr Event UpperC = Event::ShiftAlphaKey(Keyboard::Key::Log);
constexpr Event UpperD = Event::ShiftAlphaKey(Keyboard::Key::Imaginary);
constexpr Event UpperE = Event::ShiftAlphaKey(Keyboard::Key::Comma);
constexpr Event UpperF = Event::ShiftAlphaKey(Keyboard::Key::Power);

constexpr Event UpperG = Event::ShiftAlphaKey(Keyboard::Key::Sine);
constexpr Event UpperH = Event::ShiftAlphaKey(Keyboard::Key::Cosine);
constexpr Event UpperI = Event::ShiftAlphaKey(Keyboard::Key::Tangent);
constexpr Event UpperJ = Event::ShiftAlphaKey(Keyboard::Key::Pi);
constexpr Event UpperK = Event::ShiftAlphaKey(Keyboard::Key::Sqrt);
constexpr Event UpperL = Event::ShiftAlphaKey(Keyboard::Key::Square);

constexpr Event UpperM = Event::ShiftAlphaKey(Keyboard::Key::Seven);
constexpr Event UpperN = Event::ShiftAlphaKey(Keyboard::Key::Eight);
constexpr Event UpperO = Event::ShiftAlphaKey(Keyboard::Key::Nine);
constexpr Event UpperP = Event::ShiftAlphaKey(Keyboard::Key::LeftParenthesis);
constexpr Event UpperQ = Event::ShiftAlphaKey(Keyboard::Key::RightParenthesis);

constexpr Event UpperR = Event::ShiftAlphaKey(Keyboard::Key::Four);
constexpr Event UpperS = Event::ShiftAlphaKey(Keyboard::Key::Five);
constexpr Event UpperT = Event::ShiftAlphaKey(Keyboard::Key::Six);
constexpr Event UpperU = Event::ShiftAlphaKey(Keyboard::Key::Multiplication);
constexpr Event UpperV = Event::ShiftAlphaKey(Keyboard::Key::Division);

constexpr Event UpperW = Event::ShiftAlphaKey(Keyboard::Key::One);
constexpr Event UpperX = Event::ShiftAlphaKey(Keyboard::Key::Two);
constexpr Event UpperY = Event::ShiftAlphaKey(Keyboard::Key::Three);
constexpr Event UpperZ = Event::ShiftAlphaKey(Keyboard::Key::Plus);

constexpr Event shiftalphaans = Event::ShiftAlphaKey(Keyboard::Key::Ans);
// Special

constexpr Event None = Event::Special(0);
constexpr Event Termination = Event::Special(1);
constexpr Event TimerFire = Event::Special(2);
constexpr Event USBEnumeration = Event::Special(3);
constexpr Event USBPlug = Event::Special(4);
constexpr Event BatteryCharging = Event::Special(5);
/* This event is only used in the simulator, to handle text that cannot be
 * associated with a key. */
constexpr Event ExternalText = Event::Special(6);

}
}

#endif
