#ifndef ION_UNICODE_UTF8_DECODER_H
#define ION_UNICODE_UTF8_DECODER_H

#include "code_point.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>

class UnicodeDecoder {
public:
  UnicodeDecoder(size_t string, size_t initialPosition, size_t stringEnd) :
    m_string(string),
    m_stringEnd(stringEnd),
    m_stringPosition(initialPosition)
  {}
  virtual CodePoint nextCodePoint() = 0;
  virtual CodePoint previousCodePoint() = 0;
  size_t nextGlyphPosition();
  size_t previousGlyphPosition();
  size_t stringPosition() const { return m_stringPosition; }
  size_t stringEnd() const { return m_stringEnd; }

protected:
  size_t const m_string;
  size_t const m_stringEnd;
  size_t m_stringPosition;
};

/* UTF-8 encodes all valid code points using at most 4 bytes (= 28 bits), the
 * lowest codes being equal to ASCII codes. There are less than 2^21 different
 * UTF-8 valid code points.
 *
 * The encoding is the following:
 * For code points between ...   ->  The corresponding bits are ...
 * 0 and 7F         -> 0xxxxxxx
 * 80 and 7FF       -> 110xxxxx 10xxxxxx
 * 800 and FFFF     -> 1110xxxx 10xxxxxx 10xxxxxx
 * 10000 and 10FFFF -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * WARNING: A single glyph can have several code point translations. For
 * instance, 'ê' can be the code point 'e' followed by the code point '^', but
 * it can also be the unique code point 'ê'. We assume NFKD normalization, where
 * 'ê' will be 'e' followed by a combining code point '^'.
 * The i18n literals are processed by i18n.py, where we enforce the NFKD
 * normalization, but other string literals (for instance the script templates)
 * will be encoded as the text editor chooses.
 *
 * /!\ All literals except the i18n should thus avoid containing ambiguous
 *     glyphs such as accentuated letters.
 */

static_assert(sizeof(size_t) == sizeof(const char *));

class UTF8Decoder : public UnicodeDecoder {
public:
  UTF8Decoder(const char * string, const char * initialPosition = nullptr, const char * stringEnd = nullptr) :
    UnicodeDecoder(reinterpret_cast<size_t>(string), reinterpret_cast<size_t>(initialPosition == nullptr ? string : initialPosition), reinterpret_cast<size_t>(stringEnd ? stringEnd : string + strlen(string)))
    {
      assert(string != nullptr);
    }

  CodePoint nextCodePoint() override;
  CodePoint previousCodePoint() override;
  const char * nextGlyphPosition() { return reinterpret_cast<const char*>(UnicodeDecoder::nextGlyphPosition()); }
  const char * previousGlyphPosition() { return reinterpret_cast<const char*>(UnicodeDecoder::previousGlyphPosition()); }
  const char * string() const { return reinterpret_cast<const char*>(m_string); }
  const char *& stringPosition() { return reinterpret_cast<const char*&>(m_stringPosition); }
  const char * stringEnd() const { return reinterpret_cast<const char*>(m_stringEnd); }
  void setPosition(const char * position);
  constexpr static size_t CharSizeOfCodePoint(CodePoint c) {
    return c <= 0x7F ? 1 : (c <= 0x7FF ? 2 : (c <= 0xFFFF ? 3 : 4));
  }
  // No null-terminating char
  static size_t CodePointToChars(CodePoint c, char * buffer, size_t bufferLength);
  static size_t CodePointToCharsWithNullTermination(CodePoint c, char * buffer, size_t bufferSize);
  static bool IsInTheMiddleOfACodePoint(uint8_t value);
};

#endif
