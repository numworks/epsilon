#ifndef KANDINSKY_FONTS_CODE_POINTS_H
#define KANDINSKY_FONTS_CODE_POINTS_H

// [0x30a].map{|i| "0x" + i.to_s(16) +", // " + [i].pack("U") + " // " + Unicode::Name.of([i].pack("U"))}.join("|")
#include <stdint.h>


/* This array lists the code points that are rasterized by rasterizer.c. We put
 * most characters from the LATIN charset, and some mathematical characters.
 * WARNING: the array has to be sorted. */
#ifdef __cplusplus
constexpr
#endif
uint32_t CodePoints[] = {
  0x20, //   // SPACE
  0x21, // ! // EXCLAMATION MARK
  0x22, // " // QUOTATION MARK
  0x23, // # // NUMBER SIGN
  0x24, // $ // DOLLAR SIGN
  0x25, // % // PERCENT SIGN
  0x26, // & // AMPERSAND
  0x27, // ' // APOSTROPHE
  0x28, // ( // LEFT PARENTHESIS
  0x29, // ) // RIGHT PARENTHESIS
  0x2a, // * // ASTERISK
  0x2b, // + // PLUS SIGN
  0x2c, // , // COMMA
  0x2d, // - // HYPHEN-MINUS
  0x2e, // . // FULL STOP
  0x2f, // / // SOLIDUS
  0x30, // 0 // DIGIT ZERO
  0x31, // 1 // DIGIT ONE
  0x32, // 2 // DIGIT TWO
  0x33, // 3 // DIGIT THREE
  0x34, // 4 // DIGIT FOUR
  0x35, // 5 // DIGIT FIVE
  0x36, // 6 // DIGIT SIX
  0x37, // 7 // DIGIT SEVEN
  0x38, // 8 // DIGIT EIGHT
  0x39, // 9 // DIGIT NINE
  0x3a, // : // COLON
  0x3b, // ; // SEMICOLON
  0x3c, // < // LESS-THAN SIGN
  0x3d, // = // EQUALS SIGN
  0x3e, // > // GREATER-THAN SIGN
  0x3f, // ? // QUESTION MARK
  0x40, // @ // COMMERCIAL AT
  0x41, // A // LATIN CAPITAL LETTER A
  0x42, // B // LATIN CAPITAL LETTER B
  0x43, // C // LATIN CAPITAL LETTER C
  0x44, // D // LATIN CAPITAL LETTER D
  0x45, // E // LATIN CAPITAL LETTER E
  0x46, // F // LATIN CAPITAL LETTER F
  0x47, // G // LATIN CAPITAL LETTER G
  0x48, // H // LATIN CAPITAL LETTER H
  0x49, // I // LATIN CAPITAL LETTER I
  0x4a, // J // LATIN CAPITAL LETTER J
  0x4b, // K // LATIN CAPITAL LETTER K
  0x4c, // L // LATIN CAPITAL LETTER L
  0x4d, // M // LATIN CAPITAL LETTER M
  0x4e, // N // LATIN CAPITAL LETTER N
  0x4f, // O // LATIN CAPITAL LETTER O
  0x50, // P // LATIN CAPITAL LETTER P
  0x51, // Q // LATIN CAPITAL LETTER Q
  0x52, // R // LATIN CAPITAL LETTER R
  0x53, // S // LATIN CAPITAL LETTER S
  0x54, // T // LATIN CAPITAL LETTER T
  0x55, // U // LATIN CAPITAL LETTER U
  0x56, // V // LATIN CAPITAL LETTER V
  0x57, // W // LATIN CAPITAL LETTER W
  0x58, // X // LATIN CAPITAL LETTER X
  0x59, // Y // LATIN CAPITAL LETTER Y
  0x5a, // Z // LATIN CAPITAL LETTER Z
  0x5b, // [ // LEFT SQUARE BRACKET
  0x5c, // \ // REVERSE SOLIDUS
  0x5d, // ] // RIGHT SQUARE BRACKET
  0x5e, // ^ // CIRCUMFLEX ACCENT
  0x5f, // _ // LOW LINE
  0x60, // ` // GRAVE ACCENT
  0x61, // a // LATIN SMALL LETTER A
  0x62, // b // LATIN SMALL LETTER B
  0x63, // c // LATIN SMALL LETTER C
  0x64, // d // LATIN SMALL LETTER D
  0x65, // e // LATIN SMALL LETTER E
  0x66, // f // LATIN SMALL LETTER F
  0x67, // g // LATIN SMALL LETTER G
  0x68, // h // LATIN SMALL LETTER H
  0x69, // i // LATIN SMALL LETTER I
  0x6a, // j // LATIN SMALL LETTER J
  0x6b, // k // LATIN SMALL LETTER K
  0x6c, // l // LATIN SMALL LETTER L
  0x6d, // m // LATIN SMALL LETTER M
  0x6e, // n // LATIN SMALL LETTER N
  0x6f, // o // LATIN SMALL LETTER O
  0x70, // p // LATIN SMALL LETTER P
  0x71, // q // LATIN SMALL LETTER Q
  0x72, // r // LATIN SMALL LETTER R
  0x73, // s // LATIN SMALL LETTER S
  0x74, // t // LATIN SMALL LETTER T
  0x75, // u // LATIN SMALL LETTER U
  0x76, // v // LATIN SMALL LETTER V
  0x77, // w // LATIN SMALL LETTER W
  0x78, // x // LATIN SMALL LETTER X
  0x79, // y // LATIN SMALL LETTER Y
  0x7a, // z // LATIN SMALL LETTER Z
  0x7b, // { // LEFT CURLY BRACKET
  0x7c, // | // VERTICAL LINE
  0x7d, // } // RIGHT CURLY BRACKET
  0x7e, // ~ // TILDE

  0xb0, // ° // DEGREE SIGN
  0xb1, // ± // PLUS OR MINUS SIGN
  0xb7, // · // MIDDLE DOT

  0xc6, // Æ // LATIN CAPITAL LETTER AE
  0xd0, // Ð // LATIN CAPITAL LETTER ETH
  0xd7, // × // MULTIPLICATION SIGN
  0xd8, // Ø // LATIN CAPITAL LETTER O WITH STROKE
  0xde, // Þ // LATIN CAPITAL LETTER THORN
  0xdf, // ß // LATIN SMALL LETTER SHARP S
  0xe6, // æ // LATIN SMALL LETTER AE
  0xf0, // ð // LATIN SMALL LETTER ETH
  0xf7, // ÷ // DIVISION SIGN
  0xf8, // ø // LATIN SMALL LETTER O WITH STROKE
  0xfe, // þ // LATIN SMALL LETTER THORN

  0x300, //  ̀ // COMBINING GRAVE ACCENT
  0x301, //  ́ // COMBINING ACUTE ACCENT
  0x302, //  ̂ // COMBINING CIRCUMFLEX ACCENT
  0x303, //  ̃ // COMBINING TILDE
  0x305, //  ̅ // COMBINING OVERLINE
  0x308, //  ̈ // COMBINING DIAERESIS
  0x30a, //  ̊ // COMBINING RING ABOVE
  0x327, //  ̧ // COMBINING CEDILLA
  0x338, // ◌̸ // COMBINING SLASH

  0x393,   // Γ // GREEK CAPITAL LETTER GAMMA
  0x394,   // Δ // GREEK CAPITAL LETTER DELTA
  0x3a9,   // Ω // GREEK CAPITAL LETTER OMEGA
  0x3b1,   // α // GREEK SMALL LETTER ALPHA
  0x3b2,   // β // GREEK SMALL LETTER BETA
  0x3b5,   // ε // GREEK SMALL LETTER EPSILON
  0x3b8,   // θ // GREEK SMALL LETTER THETA
  0x3bb,   // λ // GREEK SMALL LETTER LAMDA
  0x3bc,   // μ // GREEK SMALL LETTER MU
  0x3c0,   // π // GREEK SMALL LETTER PI
  0x3c1,   // ρ // GREEK SMALL LETTER RHO
  0x3c3,   // σ // GREEK SMALL LETTER SIGMA
  0x3C7,   // χ // GREEK SMALL LETTER CHI
  0x1d07,  // ᴇ // LATIN LETTER SMALL CAPITAL E
  0x2192,  // → // RIGHTWARDS ARROW
  0x2211,  // ∑ // N-ARY SUMMATION
  0x221a,  // √ // SQUARE ROOT
  0x222b,  // ∫ // INTEGRAL
  0x2248,  // ≈ // ALMOST EQUAL TO
  0x2264,  // ≤ // LESS-THAN OR EQUAL TO
  0x2265,  // ≥ // GREATER-THAN OR EQUAL TO
  0x27E8,  // ⟨ // LEFT ANGLE BRACKET
  0x27E9,  // ⟩ // RIGHT ANGLE BRACKET
  0xFFFD,  // � // REPLACEMENT CHARACTER
};

const int NumberOfCodePoints = sizeof(CodePoints)/sizeof(CodePoints[0]);

#endif
