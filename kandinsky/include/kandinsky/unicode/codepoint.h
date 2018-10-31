#ifndef KANDINSKY_UNICODE_CODEPOINT_H
#define KANDINSKY_UNICODE_CODEPOINT_H

#include <stdint.h>

class Codepoint {
public:
  constexpr Codepoint(uint32_t c) : m_code(c) {}
  operator uint16_t() const { return m_code; }


  bool isCombining() const {
    return (m_code >= 0x300 && m_code <= 0x036F);
  }
private:
  uint32_t m_code;
};

static constexpr Codepoint Null = 0x0;
static constexpr Codepoint Tabulation = 0x9;
static constexpr Codepoint LineFeed = 0xA;

#endif
