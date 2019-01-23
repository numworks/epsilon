#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

namespace UTF8Helper {

static inline int minInt(int x, int y) { return x < y ? x : y; }

int CountOccurrences(const char * s, CodePoint c) {
  int count = 0;
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    /* The code point is one char long, so it is equal to its char translation.
     * We can do a classic char search. */
    const char * i = s;
    while (*i != 0) {
      if (*i == c) {
        count++;
      }
      i++;
    }
  } else {
    // The code point is more than one char long, we use a UTF8Decoder.
    UTF8Decoder decoder(s);
    CodePoint codePoint = decoder.nextCodePoint();
    while (codePoint != UCodePointNull) {
      if (codePoint == c) {
        count++;
      }
      codePoint = decoder.nextCodePoint();
    }
  }
  return count;
}

const char * CodePointSearch(const char * s, CodePoint c) {
  UTF8Decoder decoder(s);
  const char * currentPointer = s;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  while (codePoint != UCodePointNull && codePoint != c) {
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  if (codePoint == c) {
    return currentPointer;
  }
  return nullptr;
}

void CopyAndRemoveCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c, const char * * pointerToUpdate) {
  UTF8Decoder decoder(src);
  const char * currentPointer = src;
  const char * maxPointer = src + strlen(src) + 1;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  size_t bufferIndex = 0;
  size_t codePointCharSize = UTF8Decoder::CharSizeOfCodePoint(c);

  // Remove CodePoint c
  while (currentPointer < maxPointer && bufferIndex < dstSize) {
    if (codePoint != c) {
      int copySize = minInt(nextPointer - currentPointer, dstSize - bufferIndex);
      memcpy(dst + bufferIndex, currentPointer, copySize);
      bufferIndex+= copySize;
    } else if (pointerToUpdate != nullptr && currentPointer < *pointerToUpdate) {
      assert(*pointerToUpdate - src >= codePointCharSize);
      *pointerToUpdate = *pointerToUpdate - codePointCharSize;
    }
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
}

void PerformAtCodePoints(const char * s, CodePoint c, CodePointAction action, void * contextPointer, int contextInt) {
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    /* The code point is one char long, so it is equal to its char translation.
     * We can do a classic char search. */
    const char * i = s;
    while (*i != 0) {
      if (*i == c) {
        action(const_cast<char *>(i), contextPointer, contextInt);
      }
      i++;
    }
  } else {
    // The code point is more than one char long, we use a UTF8Decoder.
    UTF8Decoder decoder(s);
    const char * codePointPointer = decoder.stringPosition();
    CodePoint codePoint = decoder.nextCodePoint();
    while (codePoint != UCodePointNull) {
      if (codePoint == c) {
        action(const_cast<char *>(codePointPointer), contextPointer, contextInt);
      }
      codePointPointer = decoder.stringPosition();
      codePoint = decoder.nextCodePoint();
    }
  }
}

};
