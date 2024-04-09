#include <omg/unicode_helper.h>
#include <poincare/src/layout/rack_layout_decoder.h>

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

inline size_t CodePointSearch(const char* string, CodePoint c) {
  UTF8Decoder dec(string);
  return CodePointSearch(&dec, c);
}

inline size_t CodePointSearch(const Poincare::Internal::Tree* first, int length,
                              CodePoint c) {
  Poincare::Internal::CPLayoutDecoder dec(first, length);
  return CodePointSearch(&dec, c);
}

const Poincare::Internal::CPL* CodePointLSearch(
    const Poincare::Internal::CPL* s, CodePoint c,
    const Poincare::Internal::CPL* stop) {
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

int CompareCPLWithNullTerminatedString(const Poincare::Internal::CPL* s,
                                       int length, const char* string) {
  Poincare::Internal::CPLayoutDecoder decoder(
      reinterpret_cast<const Poincare::Internal::Tree*>(s), 0, length);
  return CompareDecoderWithNullTerminatedString(&decoder, string);
}

}  // namespace OMG
