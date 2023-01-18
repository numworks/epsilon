#ifndef ION_UNICODE_CODE_POINT_H
#define ION_UNICODE_CODE_POINT_H

#include <stdint.h>

class CodePoint {
public:
  constexpr static int MaxCodePointCharLength = sizeof(uint32_t) / sizeof(char);
  constexpr CodePoint(uint32_t c) : m_code(c) {}
  constexpr operator uint32_t() const { return m_code; }
  char getChar() const;

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
  bool isEquationOperator() const; // <, =, >, ≤, ≥
  bool isComparisonOperator() const; // <, =, >, ≤, ≥, ≠
private:
  uint32_t m_code;
};

constexpr static CodePoint UCodePointNull                     = 0x0;

// 0x1 and 0x2 are not used, so we can use them for another purpose
constexpr static CodePoint UCodePointUnknown                  = 0x1;
constexpr static CodePoint UCodePointTemporaryUnknown         = 0x2;

constexpr static CodePoint UCodePointTabulation               = 0x9;
constexpr static CodePoint UCodePointLineFeed                 = 0xa;
constexpr static CodePoint UCodePointCarriageReturn           = 0xd;

// 0x11 to 0x15 are not used, so we can use them for another purpose
constexpr static CodePoint UCodePointEmpty                    = 0x11; // Used to be parsed into EmptyExpression
/* Code points \x12 to \x14 are used to serialize expressions. UCodePointSystem
 * in particular is used to mark the next token in the parsed string as
 * reserved by the system. This allows generating arbitrary system tokens
 * (e.g. '\x14{' is a left sytem brace) or flagging identifier that should not
 * be input by the user (e.g. '\x14dep' for dependencies). System parentheses
 * could be represented by '\x14(' and '\x14)', but the alternative makes
 * serialization more compact, and we do not need \x12 and \x13 for anything
 * else. */
constexpr static CodePoint UCodePointLeftSystemParenthesis    = 0x12;
constexpr static CodePoint UCodePointRightSystemParenthesis   = 0x13;
constexpr static CodePoint UCodePointSystem                   = 0x14;

constexpr static CodePoint UCodePointDegreeSign               = 0xb0;    // °
constexpr static CodePoint UCodePointMiddleDot                = 0xb7;    // ·
constexpr static CodePoint UCodePointMultiplicationSign       = 0xd7;    // ×
constexpr static CodePoint UCodePointCombiningCircumflex      = 0x302;   // COMBINING CIRCUMFLEX
constexpr static CodePoint UCodePointCombiningOverline        = 0x305;   // COMBINING OVERLINE
constexpr static CodePoint UCodePointGreekSmallLetterTheta    = 0x3b8;   // θ
constexpr static CodePoint UCodePointGreekSmallLetterPi       = 0x3c0;   // π
constexpr static CodePoint UCodePointGreekSmallLetterSigma    = 0x3c3;   // σ
constexpr static CodePoint UCodePointGreekSmallLetterChi      = 0x3c7;   // χ
constexpr static CodePoint UCodePointLatinLetterSmallCapitalE = 0x1d07;  // ᴇ
constexpr static CodePoint UCodePointRightwardsArrow          = 0x2192;  // →
constexpr static CodePoint UCodePointNorthEastArrow           = 0x2197;  // ↗
constexpr static CodePoint UCodePointSouthEastArrow           = 0x2198;  // ↘
constexpr static CodePoint UCodePointNArySummation            = 0x2211;  // ∑
constexpr static CodePoint UCodePointSquareRoot               = 0x221a;  // √
constexpr static CodePoint UCodePointInfinity                 = 0x221e;  // ∞
constexpr static CodePoint UCodePointIntegral                 = 0x222b;  // ∫
constexpr static CodePoint UCodePointNotEqual                 = 0x2260;  // ≠
constexpr static CodePoint UCodePointInferiorEqual            = 0x2264;  // ≤
constexpr static CodePoint UCodePointSuperiorEqual            = 0x2265;  // ≥
constexpr static CodePoint UCodePointLeftAngleBracket         = 0x27E8;  // ⟨
constexpr static CodePoint UCodePointRightAngleBracket        = 0x27E9;  // ⟩
constexpr static CodePoint UCodePointReplacement              = 0xFFFD;  // �

#endif
