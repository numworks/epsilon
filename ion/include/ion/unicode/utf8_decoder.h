#ifndef ION_UNICODE_UTF8_DECODER_H
#define ION_UNICODE_UTF8_DECODER_H

#include "code_point.h"
#include <stddef.h>
#include <assert.h>

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

class UTF8Decoder {
public:
  UTF8Decoder(const char * string, const char * initialPosition = nullptr) :
    m_string(string),
    m_stringPosition(initialPosition == nullptr ? string : initialPosition)
  {
    assert(m_string != nullptr);
  }
  CodePoint nextCodePoint();
  CodePoint previousCodePoint();
  const char * nextGlyphPosition();
  const char * previousGlyphPosition();
  const char * stringPosition() const { return m_stringPosition; }
  static size_t CharSizeOfCodePoint(CodePoint c);
  static size_t CodePointToChars(CodePoint c, char * buffer, size_t bufferSize);
private:
  const char * const m_string;
  const char * m_stringPosition;
};

#endif
