#ifndef KANDINSKY_UNICODE_CODE_POINT_H
#define KANDINSKY_UNICODE_CODE_POINT_H

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
static constexpr CodePoint KDCodePointNull                     = 0x0;
static constexpr CodePoint KDCodePointTabulation               = 0x9;
static constexpr CodePoint KDCodePointLineFeed                 = 0xa;

/* 0x11, 0x12, 0x13, 0x14 represent DEVICE CONTROL ONE TO FOUR. They are not
 * used, so we can use them for another purpose */
static constexpr CodePoint KDCodePointEmpty                    = 0x11; // Used to be parsed into EmptyExpression
static constexpr CodePoint KDCodePointLeftSuperscript          = 0x12; // Used to parse Power
static constexpr CodePoint KDCodePointRightSuperscript         = 0x13; // Used to parse Power
static constexpr CodePoint KDCodePointUnknownX                 = 0x14; // Used to store expressions

static constexpr CodePoint KDCodePointSpace                    = 0x20;    //
static constexpr CodePoint KDCodePointDegree                   = 0xb0;    // °
static constexpr CodePoint KDCodePointMiddleDot                = 0xb7;    // ·
static constexpr CodePoint KDCodePointMultiplicationSign       = 0xd7;    // ×
static constexpr CodePoint KDCodePointGreekCapitalLetterGamma  = 0x393;   // Γ
static constexpr CodePoint KDCodePointGreekCapitalLetterDelta  = 0x394;   // Δ
static constexpr CodePoint KDCodePointGreekSmallLetterTheta    = 0x3b8;   // θ
static constexpr CodePoint KDCodePointGreekSmallLetterLambda   = 0x3bb;   // λ
static constexpr CodePoint KDCodePointGreekSmallLetterPi       = 0x3c0;   // π
static constexpr CodePoint KDCodePointGreekSmallLetterSigma    = 0x3c3;   // σ
static constexpr CodePoint KDCodePointLatinLetterSmallCapitalE = 0x1d07;  // ᴇ
static constexpr CodePoint KDCodePointScriptSmallE             = 0x212f;  // ℯ
static constexpr CodePoint KDCodePointRightwardsArrow          = 0x2192;  // →
static constexpr CodePoint KDCodePointNArySummation            = 0x2211;  // ∑
static constexpr CodePoint KDCodePointSquareRoot               = 0x221a;  // √
static constexpr CodePoint KDCodePointIntegral                 = 0x222b;  // ∫
static constexpr CodePoint KDCodePointAlmostEqualTo            = 0x2248;  // ≈
static constexpr CodePoint KDCodePointLessThanOrEqualTo        = 0x2264;  // ≤
static constexpr CodePoint KDCodePointGreaterThanOrEqualTo     = 0x2265;  // ≥
static constexpr CodePoint KDCodePointMathematicalBoldSmallI   = 0x1d422; // 𝐢

#endif
