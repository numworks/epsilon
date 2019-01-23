#ifndef ION_UNICODE_CODE_POINT_H
#define ION_UNICODE_CODE_POINT_H

#include <stdint.h>

class CodePoint {
public:
  constexpr static int MaxCodePointCharLength = sizeof(uint32_t) / sizeof(char);
  constexpr CodePoint(uint32_t c) : m_code(c) {}
  constexpr operator uint32_t() const { return m_code; }

  bool isCombining() const {
    return (m_code >= 0x300 && m_code <= 0x036F);
  }
private:
  uint32_t m_code;
};


// TODO LEA Remove unneeded values
static constexpr CodePoint UCodePointNull                     = 0x0;
static constexpr CodePoint UCodePointTabulation               = 0x9;
static constexpr CodePoint UCodePointLineFeed                 = 0xa;

/* 0x11, 0x12, 0x13, 0x14 represent DEVICE CONTROL ONE TO FOUR. They are not
 * used, so we can use them for another purpose */
static constexpr CodePoint UCodePointEmpty                    = 0x11; // Used to be parsed into EmptyExpression
static constexpr CodePoint UCodePointLeftSuperscript          = 0x12; // Used to parse Power
static constexpr CodePoint UCodePointRightSuperscript         = 0x13; // Used to parse Power
static constexpr CodePoint UCodePointUnknownX                 = 0x14; // Used to store expressions

static constexpr CodePoint UCodePointSpace                    = 0x20;    //
static constexpr CodePoint UCodePointDegree                   = 0xb0;    // °
static constexpr CodePoint UCodePointMiddleDot                = 0xb7;    // ·
static constexpr CodePoint UCodePointMultiplicationSign       = 0xd7;    // ×
static constexpr CodePoint UCodePointGreekCapitalLetterGamma  = 0x393;   // Γ
static constexpr CodePoint UCodePointGreekCapitalLetterDelta  = 0x394;   // Δ
static constexpr CodePoint UCodePointGreekSmallLetterTheta    = 0x3b8;   // θ
static constexpr CodePoint UCodePointGreekSmallLetterLambda   = 0x3bb;   // λ
static constexpr CodePoint UCodePointGreekSmallLetterPi       = 0x3c0;   // π
static constexpr CodePoint UCodePointGreekSmallLetterSigma    = 0x3c3;   // σ
static constexpr CodePoint UCodePointLatinLetterSmallCapitalE = 0x1d07;  // ᴇ
static constexpr CodePoint UCodePointScriptSmallE             = 0x212f;  // ℯ
static constexpr CodePoint UCodePointRightwardsArrow          = 0x2192;  // →
static constexpr CodePoint UCodePointNArySummation            = 0x2211;  // ∑
static constexpr CodePoint UCodePointSquareRoot               = 0x221a;  // √
static constexpr CodePoint UCodePointIntegral                 = 0x222b;  // ∫
static constexpr CodePoint UCodePointAlmostEqualTo            = 0x2248;  // ≈
static constexpr CodePoint UCodePointLessThanOrEqualTo        = 0x2264;  // ≤
static constexpr CodePoint UCodePointGreaterThanOrEqualTo     = 0x2265;  // ≥
static constexpr CodePoint UCodePointMathematicalBoldSmallI   = 0x1d422; // 𝐢

#endif
