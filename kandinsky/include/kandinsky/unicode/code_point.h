#ifndef KANDINSKY_UNICODE_CODE_POINT_H
#define KANDINSKY_UNICODE_CODE_POINT_H

#include <stdint.h>

class CodePoint {
public:
  constexpr CodePoint(uint32_t c) : m_code(c) {}
  operator uint16_t() const { return m_code; }


  bool isCombining() const {
    return (m_code >= 0x300 && m_code <= 0x036F);
  }
private:
  uint32_t m_code;
};

static constexpr CodePoint KDCodePointNull = 0x0;
static constexpr CodePoint KDCodePointTabulation = 0x9;
static constexpr CodePoint KDCodePointLineFeed = 0xA;

#endif
