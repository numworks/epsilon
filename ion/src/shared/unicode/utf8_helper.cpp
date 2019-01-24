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
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    const char * result = s;
    while (*result != 0 && *result != c) {
      result++;
    }
    return result;
  }
  UTF8Decoder decoder(s);
  const char * currentPointer = s;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  while (codePoint != UCodePointNull && codePoint != c) {
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  return currentPointer;
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

const char * PerformAtCodePoints(const char * s, CodePoint c, CodePointAction actionCodePoint, CodePointAction actionOtherCodePoint, void * contextPointer, int contextInt, CodePoint stoppingCodePoint, bool goingRight, const char * initialPosition) {
  /* If we are decoding towards the left, we must have a starting position. If
   * we are decoding towards the right, the starting position is the start of
   * string. */
  assert((goingRight && initialPosition == nullptr)
      || (!goingRight && initialPosition != nullptr));

  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1 && UTF8Decoder::CharSizeOfCodePoint(stoppingCodePoint) == 1) {
    /* The code points are one char long, so they are equal to their char
     * translations. We can do a classic char search. */
    if (goingRight) {
      const char * i = s;
      while (*i != stoppingCodePoint && *i != 0) {
        if (*i == c) {
          actionCodePoint(const_cast<char *>(i), contextPointer, contextInt);
        } else {
          actionOtherCodePoint(const_cast<char *>(i), contextPointer, contextInt);
        }
        i++;
      }
      return i;
    }
    const char * i = initialPosition - 1;
    while (i >= s && *i != stoppingCodePoint) {
      if (*i == c) {
        actionCodePoint(const_cast<char *>(i), contextPointer, contextInt);
      } else {
        actionOtherCodePoint(const_cast<char *>(i), contextPointer, contextInt);
      }
      i--;
    }
    return i;
  }
  // The code point is more than one char long, we use a UTF8Decoder.
  if (goingRight) {
    UTF8Decoder decoder(s);
    const char * codePointPointer = decoder.stringPosition();
    CodePoint codePoint = decoder.nextCodePoint();
    while (codePoint != stoppingCodePoint && codePoint != UCodePointNull) {
      if (codePoint == c) {
        actionCodePoint(const_cast<char *>(codePointPointer), contextPointer, contextInt);
      } else {
        actionOtherCodePoint(const_cast<char *>(codePointPointer), contextPointer, contextInt);
      }
      codePointPointer = decoder.stringPosition();
      codePoint = decoder.nextCodePoint();
    }
    return codePointPointer;
  }
  assert(!goingRight);
  if (initialPosition <= s) {
    return initialPosition;
  }
  UTF8Decoder decoder(s, initialPosition);
  CodePoint codePoint = decoder.previousCodePoint();
  const char * codePointPointer = decoder.stringPosition();
  while (codePointPointer >= s && codePoint != stoppingCodePoint) {
    if (codePoint == c) {
      actionCodePoint(const_cast<char *>(codePointPointer), contextPointer, contextInt);
    } else {
      actionOtherCodePoint(const_cast<char *>(codePointPointer), contextPointer, contextInt);
    }
    if (codePointPointer > s) {
      codePoint = decoder.previousCodePoint();
      codePointPointer = decoder.stringPosition();
    } else {
      /* If the current pointer is s, we cannot continue decoding. Decreasing s
       * will stop the while loop. */
      codePointPointer = s-1;
    }
  }
  return codePointPointer;
}

bool PreviousCodePointIs(const char * buffer, const char * location, CodePoint c) {
  assert(location > buffer);
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    return *(location -1) == c;
  }
  UTF8Decoder decoder(buffer, location);
  return decoder.previousCodePoint() == c;
}

bool CodePointIs(const char * location, CodePoint c) {
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    return *(location) == c;
  }
  UTF8Decoder decoder(location);
  return decoder.nextCodePoint() == c;
}

int RemovePreviousCodePoint(const char * text, char * location, CodePoint * c) {
  assert(c != nullptr);
  if (location <= text) {
    assert(location == text);
    return 0;
  }

  // Find the previous code point
  UTF8Decoder decoder(text, location);
  *c = decoder.previousCodePoint();

  // Shift the buffer
  int codePointSize = UTF8Decoder::CharSizeOfCodePoint(*c);
  char * iterator = location - codePointSize;
  assert(iterator >= text);
  do {
    *iterator = *(iterator + codePointSize);
    iterator++;
  } while (*(iterator - 1) != 0); // Stop shifting after writing a null terminating char.

  return codePointSize;
}

};
