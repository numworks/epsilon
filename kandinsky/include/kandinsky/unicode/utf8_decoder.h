#ifndef KANDINSKY_UNICODE_UTF8_DECODER_H
#define KANDINSKY_UNICODE_UTF8_DECODER_H

#include <stddef.h>
#include "code_point.h"

/* UTF-8 encodes all valid code points using at most 4 bytes (= 28 bits), the
 * lowest codes being equal to ASCII codes. There are less than 2^21 different
 * UTF-8 valid code points.
 *
 * The encoding is the following:
 * For code points between ...   ->  The corresponding bits are ...
 * 0 and 7F         -> 0xxxxxxx
 * 80 and 7FF       -> 110xxxxx 10xxxxxx
 * 800 and FFFF     -> 1110xxxx 10xxxxxx 10xxxxxx
 * 10000 and 10FFFF -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */

class UTF8Decoder {
public:
  UTF8Decoder(const char * string) : m_string(string) {}
  CodePoint nextCodePoint();
  const char * stringPosition() const { return m_string; }
  static size_t CharSizeOfCodePoint(CodePoint c);
  static size_t CodePointToChars(CodePoint c, char * buffer, int bufferSize);
private:
  const char * m_string;
};

#endif
