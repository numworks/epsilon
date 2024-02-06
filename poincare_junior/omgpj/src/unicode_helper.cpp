#include <omgpj/unicode_helper.h>
#include <poincare_junior/src/layout/rack_layout_decoder.h>

namespace OMG {

size_t CodePointSearch(UnicodeDecoder* decoder, CodePoint c) {
  while (CodePoint codePoint = decoder->nextCodePoint()) {
    if (codePoint == c) {
      return decoder->position() - 1;
    }
  }
  decoder->previousCodePoint();
  return decoder->position();
}

const PoincareJ::CPL* CodePointLSearch(const PoincareJ::CPL* s, CodePoint c,
                                       const PoincareJ::CPL* stop) {
  while (s != stop && *s != 0) {
    if (*s == c) {
      return s;
    }
    s++;
  }
  return s;
}

int CompareDecoders(UnicodeDecoder* a, UnicodeDecoder* b) {
  while (CodePoint c = a->nextCodePoint()) {
    CodePoint d = b->nextCodePoint();
    if (c != d) {
      return c - d;
    }
  }
  return b->nextCodePoint();
}

int CompareDecoderWithNullTerminatedString(UnicodeDecoder* decoder,
                                           const char* string) {
  // TODO this UnicodeDecoder API is aweful
  size_t position = decoder->position();
  UTF8Decoder stringDecoder(string);
  int result = CompareDecoders(decoder, &stringDecoder);
  decoder->unsafeSetPosition(position);
  return result;
}

}  // namespace OMG
