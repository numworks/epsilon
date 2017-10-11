#ifndef CODE_CONSOLE_LINE_H
#define CODE_CONSOLE_LINE_H

#include <stddef.h>

namespace Code {

class ConsoleLine {
public:
  enum class Type {
    Command = 0,
    Result = 1
  };
  ConsoleLine(Type type = Type::Command, const char * text = nullptr) :
    m_type(type), m_text(text) {}
  Type type() const { return m_type; }
  const char * text() const { return m_text; }
  static inline size_t sizeOfConsoleLine(size_t textLength) {
    return 1 + textLength + 1; // Marker, text, null termination
  }
private:
  Type m_type;
  const char * m_text;
};

}

#endif
