#ifndef ION_UNICODE_UTF8_DECODER_H
#define ION_UNICODE_UTF8_DECODER_H

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "code_point.h"

class UnicodeDecoder {
 public:
  UnicodeDecoder(size_t initialPosition, size_t end)
      : m_position(initialPosition), m_end(end) {}
  virtual CodePoint nextCodePoint() = 0;
  virtual CodePoint previousCodePoint() = 0;
  size_t nextGlyphPosition();
  size_t previousGlyphPosition();
  size_t position() const { return m_position; }
  size_t start() const { return 0; }
  size_t end() const { return m_end; }
  void unsafeSetPosition(size_t position) { m_position = position; }
  size_t printInBuffer(char* buffer, size_t bufferSize,
                       size_t printLength = -1);

 protected:
  size_t m_position;
  size_t m_end;
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

class UTF8Decoder : public UnicodeDecoder {
 public:
  UTF8Decoder(const char* string, const char* initialPosition = nullptr,
              const char* stringEnd = nullptr)
      : UnicodeDecoder(initialPosition ? initialPosition - string : 0,
                       stringEnd ? stringEnd - string : strlen(string)),
        m_string(string) {
    assert(string != nullptr);
  }

  CodePoint nextCodePoint() override;
  CodePoint previousCodePoint() override;
  const char* nextGlyphPosition() {
    return m_string + UnicodeDecoder::nextGlyphPosition();
  }
  const char* previousGlyphPosition() {
    return m_string + UnicodeDecoder::previousGlyphPosition();
  }
  const char* string() const { return m_string; }
  const char* stringPosition() { return m_string + m_position; }
  const char* stringEnd() const { return m_string + m_end; }
  void setPosition(const char* position);
  constexpr static size_t CharSizeOfCodePoint(CodePoint c) {
    return c <= 0x7F ? 1 : (c <= 0x7FF ? 2 : (c <= 0xFFFF ? 3 : 4));
  }
  // No null-terminating char
  static size_t CodePointToChars(CodePoint c, char* buffer,
                                 size_t bufferLength);
  static size_t CodePointToCharsWithNullTermination(CodePoint c, char* buffer,
                                                    size_t bufferSize);
  static bool IsInTheMiddleOfACodePoint(uint8_t value);

 private:
  char nextByte() { return m_string[m_position++]; }
  char previousByte() { return m_string[--m_position]; }
  const char* m_string;
};

#endif
