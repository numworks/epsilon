#ifndef ION_EVENTS_H
#define ION_EVENTS_H

#include <ion/keyboard.h>
#include <omg/directions.h>

#if ION_EVENTS_JOURNAL
#include <string.h>
#endif

namespace Ion {
namespace Events {

class ShiftAlphaStatus {
 public:
  enum class ShiftStatus : bool { Inactive = false, Active = true };
  enum class AlphaStatus : uint8_t {
    Inactive = 0,
    Active,
    Locked,
    NumberOfStatuses
  };

  constexpr ShiftAlphaStatus(ShiftStatus shift, AlphaStatus alpha)
      : m_shift(shift), m_alpha(alpha) {}

  constexpr ShiftAlphaStatus()
      : ShiftAlphaStatus(ShiftStatus::Inactive, AlphaStatus::Inactive) {}

  bool operator==(const ShiftAlphaStatus& other) const {
    return m_shift == other.m_shift && m_alpha == other.m_alpha;
  }
  bool operator!=(const ShiftAlphaStatus& other) const {
    return !(*this == other);
  }

  bool shiftIsActive() const { return m_shift != ShiftStatus::Inactive; }
  bool alphaIsActive() const { return m_alpha != AlphaStatus::Inactive; }
  bool alphaIsLocked() const { return m_alpha == AlphaStatus::Locked; }

  void removeShift() { m_shift = ShiftStatus::Inactive; }
  void removeAlpha() { m_alpha = AlphaStatus::Inactive; }

  void toggleShift() {
    m_shift = static_cast<ShiftStatus>(!static_cast<bool>(m_shift));
  }
  void cycleAlpha() {
    m_alpha = static_cast<AlphaStatus>(
        (static_cast<uint8_t>(m_alpha) + 1) %
        static_cast<uint8_t>(AlphaStatus::NumberOfStatuses));
    if (m_alpha == AlphaStatus::Inactive) {
      removeShift();
    }
  }

 private:
  /* This is implemented as an uint8_t so that only 1 register has to be passed
   * through SVCalls. */
  ShiftStatus m_shift : 1;
  AlphaStatus m_alpha : 7;
};
static_assert(sizeof(ShiftAlphaStatus) == sizeof(uint8_t));

class Event {
 public:
  constexpr static int k_pageSize = Keyboard::NumberOfKeys;
  constexpr static int k_plainEventsOffset = 0;
  constexpr static int k_shiftEventsOffset = 1 * k_pageSize;
  constexpr static int k_alphaEventsOffset = 2 * k_pageSize;
  constexpr static int k_shiftAlphaEventsOffset = 3 * k_pageSize;
  constexpr static int k_specialEventsOffset = 4 * k_pageSize;
  constexpr static Event PlainKey(Keyboard::Key k) { return Event((int)k); }
  constexpr static Event ShiftKey(Keyboard::Key k) {
    return Event(k_shiftEventsOffset + (int)k);
  }
  constexpr static Event AlphaKey(Keyboard::Key k) {
    return Event(k_alphaEventsOffset + (int)k);
  }
  constexpr static Event ShiftAlphaKey(Keyboard::Key k) {
    return Event(k_shiftAlphaEventsOffset + (int)k);
  }
  constexpr static Event Special(int i) {
    return Event(k_specialEventsOffset + i);
  }

  // Return Ion::Event::None by default
  constexpr Event() : m_id(k_specialEventsOffset) {}
  constexpr Event(int i) : m_id(i) { assert(static_cast<int>(m_id) == i); }
  constexpr explicit operator uint8_t() const { return m_id; }

  Event(Keyboard::Key key, bool shift, bool alpha, bool lock);

  bool operator==(const Event& other) const { return (m_id == other.m_id); }
  bool operator!=(const Event& other) const { return (m_id != other.m_id); }
  bool isKeyboardEvent() const { return m_id < k_specialEventsOffset; }
  bool isSpecialEvent() const { return m_id >= k_specialEventsOffset; }
  bool isMoveEvent() const;
  bool isSelectionEvent() const;
  bool isKeyPress() const;
  bool isRepeatable() const;

  // Return the length of the copied text (and not the size)
  const char* text() const;
#if DEBUG
  const char* name() const;
#endif

  OMG::Direction direction() const;

 private:
  const char* defaultText() const;
  uint8_t m_id;
};

#if ION_EVENTS_JOURNAL
class Journal {
 public:
  virtual void pushEvent(Event e) = 0;
  virtual Event popEvent() = 0;
  virtual bool isEmpty() = 0;
  const char* startingLanguage() const { return m_startingLanguage; }
  void setStartingLanguage(const char* language) {
    strlcpy(m_startingLanguage, language, k_languageSize);
  }
  constexpr static int k_languageSize = 3;

 private:
  char m_startingLanguage[k_languageSize] = {0};
};

void replayFrom(Journal* l);
void logTo(Journal* l);
#endif

Event getEvent(int* timeout);
size_t copyText(uint8_t eventId, char* buffer, size_t bufferSize);
bool isDefined(uint8_t eventId);
ShiftAlphaStatus shiftAlphaStatus();
void setShiftAlphaStatus(ShiftAlphaStatus s);
void setSpinner(bool spinner);
int longPressCounter();
inline bool isRepeating() { return longPressCounter() > 0; }
inline int longPressFactor() {
  // The long press factor is increased by 4 every 20 loops in getEvent(2 sec)
  return (longPressCounter() / 20) * 4 + 1;
}

// Keyboard events

#define DUMMY_KEY(...) PLAIN_KEY(__VA_ARGS__)
#define PLAIN_KEY(Name, ...) \
  constexpr Event Name = Event::PlainKey(Keyboard::Key::Name);

#define SHIFT_KEY(Name, R, C, SEvent, ...)                     \
  constexpr Event Name = Event::PlainKey(Keyboard::Key::Name); \
  constexpr Event SEvent = Event::ShiftKey(Keyboard::Key::Name);

#define ALPHA_KEY(Name, R, C, AEvent, SAEvent, ...)              \
  constexpr Event Name = Event::PlainKey(Keyboard::Key::Name);   \
  constexpr Event AEvent = Event::AlphaKey(Keyboard::Key::Name); \
  constexpr Event SAEvent = Event::ShiftAlphaKey(Keyboard::Key::Name);

#define SHIFT_ALPHA_KEY(Name, R, C, SEvent, AEvent, SAEvent, ...) \
  constexpr Event Name = Event::PlainKey(Keyboard::Key::Name);    \
  constexpr Event SEvent = Event::ShiftKey(Keyboard::Key::Name);  \
  constexpr Event AEvent = Event::AlphaKey(Keyboard::Key::Name);  \
  constexpr Event SAEvent = Event::ShiftAlphaKey(Keyboard::Key::Name);

#include <ion/keys.inc>

#undef PLAIN_KEY
#undef SHIFT_KEY
#undef ALPHA_KEY
#undef SHIFT_ALPHA_KEY
#undef DUMMY_KEY

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
/* This event is fired one time after the getEvent did not find any event. */
constexpr Event Idle = Event::Special(7);

inline bool Event::isMoveEvent() const {
  return *this == Left || *this == Right || *this == Up || *this == Down;
}

inline bool Event::isSelectionEvent() const {
  return *this == ShiftLeft || *this == ShiftRight || *this == ShiftUp ||
         *this == ShiftDown;
}

inline bool Event::isKeyPress() const {
  return isKeyboardEvent() || *this == ExternalText;
}

inline bool Event::isRepeatable() const {
  return isMoveEvent() || isSelectionEvent() || *this == Events::Back ||
         *this == Events::Backspace;
}

inline OMG::Direction Event::direction() const {
  assert(isMoveEvent() || isSelectionEvent());
  return (*this == Ion::Events::Left || *this == Ion::Events::ShiftLeft)
             ? OMG::Direction::Left()
         : (*this == Ion::Events::Up || *this == Ion::Events::ShiftUp)
             ? OMG::Direction::Up()
         : (*this == Ion::Events::Down || *this == Ion::Events::ShiftDown)
             ? OMG::Direction::Down()
             : OMG::Direction::Right();
}

}  // namespace Events
}  // namespace Ion

#endif
