#include <omg/unicode_helper.h>

namespace OMG {

size_t CodePointSearch(ForwardUnicodeDecoder* decoder, CodePoint c) {
  while (CodePoint codePoint = decoder->codePoint()) {
    if (codePoint == c) {
      return decoder->position();
    }
    decoder->nextCodePoint();
  }
  return decoder->position();
}

size_t CodePointSearch(const char* string, CodePoint c) {
  UTF8Decoder decoder(string);
  size_t result = CodePointSearch(&decoder, c);
  if (result == static_cast<size_t>(-1)) {
    return strlen(string);
  }
  return result;
}

int CompareDecoders(ForwardUnicodeDecoder* a, ForwardUnicodeDecoder* b) {
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
