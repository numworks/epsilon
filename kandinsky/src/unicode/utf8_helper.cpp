#include <kandinsky/unicode/utf8_helper.h>
#include <kandinsky/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

namespace UTF8Helper {

static inline int min(int x, int y) { return x < y ? x : y; }

const char * CodePointSearch(const char * s, CodePoint c) {
  UTF8Decoder decoder(s);
  const char * currentPointer = s;
  const char * nextPointer = decoder.nextCodePointPointer();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != KDCodePointNull && codePoint != c) {
    currentPointer = nextPointer;
    nextPointer = decoder.nextCodePointPointer();
    codePoint = decoder.nextCodePoint();
  }
  if (codePoint == c) {
    return currentPointer;
  }
  return nullptr;
}

void CopyAndRemoveCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c, size_t * indexToUpdate) {
  UTF8Decoder decoder(src);
  const char * currentPointer = src;
  const char * nextPointer = decoder.nextCodePointPointer();
  const char * maxPointer = src + strlen(src) + 1;
  CodePoint codePoint = decoder.nextCodePoint();
  size_t bufferIndex = 0;
  size_t codePointCharSize = UTF8Decoder::CharSizeOfCodePoint(c);

  // Remove CodePoint c
  while (currentPointer < maxPointer && bufferIndex < dstSize) {
    if (codePoint != c) {
      int copySize = min(nextPointer - currentPointer, dstSize - bufferIndex);
      memcpy(dst + bufferIndex, currentPointer, copySize);
      bufferIndex+= copySize;
    } else if (indexToUpdate != nullptr && currentPointer - src < *indexToUpdate) {
      assert(*indexToUpdate >= codePointCharSize);
      *indexToUpdate-= codePointCharSize;
    }
    currentPointer = nextPointer;
    nextPointer = decoder.nextCodePointPointer();
    codePoint = decoder.nextCodePoint();
  }
}

};
