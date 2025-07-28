#ifndef CODE_CONSOLE_LINE_H
#define CODE_CONSOLE_LINE_H

#include <stddef.h>

namespace Code {

class ConsoleLine {
 public:
  enum class Type {
    CurrentSessionCommand = 0,
    CurrentSessionResult = 1,
    PreviousSessionCommand = 2,
    PreviousSessionResult = 3
  };
  ConsoleLine(Type type = Type::CurrentSessionCommand,
              const char* text = nullptr)
      : m_type(type), m_text(text) {}
  Type type() const { return m_type; }
  const char* text() const { return m_text; }
  bool isFromCurrentSession() const {
    return m_type == Type::CurrentSessionCommand ||
           m_type == Type::CurrentSessionResult;
  }
  bool isCommand() const {
    return m_type == Type::CurrentSessionCommand ||
           m_type == Type::PreviousSessionCommand;
  }
  bool isResult() const {
    return m_type == Type::CurrentSessionResult ||
           m_type == Type::PreviousSessionResult;
  }
  static inline size_t sizeOfConsoleLine(size_t textLength) {
    return 1 + textLength + 1;  // Marker, text, null termination
  }

 private:
  Type m_type;
  const char* m_text;
};

}  // namespace Code

#endif
