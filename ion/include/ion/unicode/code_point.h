#ifndef ION_UNICODE_CODE_POINT_H
#define ION_UNICODE_CODE_POINT_H

#include <stdint.h>

class CodePoint {
public:
  constexpr static int MaxCodePointCharLength = sizeof(uint32_t) / sizeof(char);
  constexpr CodePoint(uint32_t c) : m_code(c) {}
  constexpr operator uint32_t() const { return m_code; }

  bool isBinaryDigit() const {
    return '0' <= m_code && m_code <= '1';
  }
  bool isDecimalDigit() const {
    return '0' <= m_code && m_code <= '9';
  }
  bool isHexadecimalDigit() const {
    return isDecimalDigit() || ('A' <= m_code && m_code <= 'F') || ('a' <= m_code && m_code <= 'f');
  }
  bool isLatinCapitalLetter() const {
    return 'A' <= m_code && m_code <= 'Z';
  }
  bool isLatinSmallLetter() const {
    return 'a' <= m_code && m_code <= 'z';
  }
  bool isLatinLetter() const {
    return isLatinCapitalLetter() || isLatinSmallLetter();
  }
  bool isCombining() const {
    return (m_code >= 0x300 && m_code <= 0x036F);
  }
  bool isGreekCapitalLetter() const {
    return 0x391 <= m_code && m_code <= 0x3a9 && m_code != 0x3a2;
  }
  bool isGreekSmallLetter() const {
    return 0x3b1 <= m_code && m_code <= 0x3c9;
  }
private:
  uint32_t m_code;
};

static constexpr CodePoint UCodePointNull                     = 0x0;

// 0x1 represents soh. It is not used, so we can use it for another purpose.
static constexpr CodePoint UCodePointUnknown                  = 0x1;

static constexpr CodePoint UCodePointTabulation               = 0x9;
static constexpr CodePoint UCodePointLineFeed                 = 0xa;

// 0x11 to 0x15 are not used, so we can use them for another purpose
static constexpr CodePoint UCodePointEmpty                    = 0x11; // Used to be parsed into EmptyExpression
static constexpr CodePoint UCodePointLeftSystemParenthesis    = 0x12; // Used for serialization
static constexpr CodePoint UCodePointRightSystemParenthesis   = 0x13; // Used for serialization

static constexpr CodePoint UCodePointMiddleDot                = 0xb7;    // ·
static constexpr CodePoint UCodePointMultiplicationSign       = 0xd7;    // ×
static constexpr CodePoint UCodePointGreekSmallLetterTheta    = 0x3b8;   // θ
static constexpr CodePoint UCodePointGreekSmallLetterPi       = 0x3c0;   // π
static constexpr CodePoint UCodePointLatinLetterSmallCapitalE = 0x1d07;  // ᴇ
static constexpr CodePoint UCodePointScriptSmallE             = 0x212f;  // ℯ
static constexpr CodePoint UCodePointRightwardsArrow          = 0x2192;  // →
static constexpr CodePoint UCodePointNArySummation            = 0x2211;  // ∑
static constexpr CodePoint UCodePointSquareRoot               = 0x221a;  // √
static constexpr CodePoint UCodePointIntegral                 = 0x222b;  // ∫
static constexpr CodePoint UCodePointMathematicalBoldSmallI   = 0x1d422; // 𝐢

#endif
