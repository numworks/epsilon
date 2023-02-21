#ifndef ION_KEYBOARD_EVENT_DATA_H
#define ION_KEYBOARD_EVENT_DATA_H

#include <stddef.h>

namespace Ion {
namespace Events {

class EventData {
 public:
  constexpr static size_t k_maxDataSize = 10;
  constexpr static EventData Undefined() { return EventData(nullptr); }
  constexpr static EventData Textless() { return EventData(""); }
  constexpr static EventData Text(const char* text) { return EventData(text); }
  bool isDefined() const { return (m_data != nullptr); }
  const char* text() const;

 private:
  constexpr EventData(const char* data) : m_data(data) {}
  const char* m_data;
};

}  // namespace Events
}  // namespace Ion

#endif
