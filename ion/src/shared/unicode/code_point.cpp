#include <ion/unicode/code_point.h>
#include <ion/unicode/utf8_decoder.h>
#include <assert.h>

char CodePoint::getChar() const {
  assert(UTF8Decoder::CharSizeOfCodePoint(*this) == 1);
  return static_cast<char>(m_code);
}