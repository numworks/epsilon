#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace UTF8Helper {

int CountOccurrences(const char * s, CodePoint c) {
  assert(c != UCodePointNull);
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

bool HasCodePoint(const char * s, CodePoint c) {
  assert(c != 0);
  return *CodePointSearch(s, c) != 0;
}

const char * NotCodePointSearch(const char * s, CodePoint c, bool goingLeft, const char * initialPosition) {
  if (goingLeft && initialPosition == s) {
    return s;
  }
  assert(goingLeft || initialPosition == nullptr);
  assert(!goingLeft || initialPosition != nullptr);
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    /* The code points are one char long, so they are equal to their char
     * translations. We can do a classic char search. */
    const char * codePointPointer = goingLeft ? initialPosition - 1 : s;
    while ((goingLeft ?codePointPointer > s : *codePointPointer != 0) && *codePointPointer == c) {
      codePointPointer += goingLeft ? -1 : 1;
    }
    return codePointPointer;
  }
  if (goingLeft) {
    UTF8Decoder decoder(s, initialPosition);
    CodePoint codePoint = decoder.previousCodePoint();
    const char * codePointPointer = decoder.stringPosition();
    while (codePointPointer > s && codePoint == c) {
      codePoint = decoder.previousCodePoint();
      codePointPointer = decoder.stringPosition();
    }
    return codePointPointer;
  }
  UTF8Decoder decoder(s);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && codePoint == c) {
    codePointPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return codePointPointer;
}

bool CopyAndRemoveCodePoints(char * dst, size_t dstSize, const char * src, CodePoint * codePoints, int numberOfCodePoints) {
  UTF8Decoder decoder(src);
  CodePoint codePoint = decoder.nextCodePoint();
  if (dstSize <= 0) {
    return codePoint == UCodePointNull;
  }
  assert(numberOfCodePoints >= 1);
  const char * currentPointer = src;
  const char * nextPointer = decoder.stringPosition();
  size_t bufferIndex = 0;

  // Remove CodePoint c
  while (codePoint != UCodePointNull && bufferIndex < dstSize) {
    bool remove = false;
    for (int i = 0; i < numberOfCodePoints; i++) {
      if (codePoint == codePoints[i]) {
        remove = true;
        break;
      }
    }
    if (!remove) {
      size_t copySize = nextPointer - currentPointer;
      if (copySize > dstSize - 1 - bufferIndex) {
        // Copying the current code point to the buffer would overflow the buffer
        break;
      }
      memcpy(dst + bufferIndex, currentPointer, copySize);
      bufferIndex+= copySize;
    }
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  *(dst + bufferIndex) = 0;
  return codePoint == UCodePointNull;
}

void RemoveCodePoint(char * buffer, CodePoint c, const char * * pointerToUpdate, const char * stoppingPosition) {
  UTF8Decoder decoder(buffer);
  const char * currentPointer = buffer;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * initialPointerToUpdate =  *pointerToUpdate;
  const char * nextPointer = decoder.stringPosition();
  size_t bufferIndex = 0;
  int codePointCharSize = UTF8Decoder::CharSizeOfCodePoint(c);
  (void)codePointCharSize; // Silence compilation warning about unused variable.

  while (codePoint != UCodePointNull && (stoppingPosition == nullptr || currentPointer < stoppingPosition)) {
    if (codePoint != c) {
      int copySize = nextPointer - currentPointer;
      memmove(buffer + bufferIndex, currentPointer, copySize);
      bufferIndex+= copySize;
    } else if (pointerToUpdate != nullptr && currentPointer < initialPointerToUpdate) {
      assert(*pointerToUpdate - buffer >= codePointCharSize);
      *pointerToUpdate = *pointerToUpdate - codePointCharSize;
    }
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  if (codePoint == UCodePointNull) {
    *(buffer + bufferIndex) = 0;
  } else {
    assert(stoppingPosition != nullptr);
    // Find the null-terminating code point
    const char * nullTermination = currentPointer + strlen(currentPointer);
    /* Copy what remains of the buffer after the stopping position for code
     * point removal */
    memmove(buffer + bufferIndex, stoppingPosition, nullTermination - stoppingPosition + 1);
  }
}

size_t CopyUntilCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c) {
  UTF8Decoder decoder(src);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull && codePoint != c) {
    codePointPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  assert(codePointPointer >= src);
  size_t copySize = std::min(dstSize - 1, static_cast<size_t>(codePointPointer - src));
  assert(UTF8Helper::CodePointIs(src + copySize, 0) || UTF8Helper::CodePointIs(src + copySize, c));
  memmove(dst, src, copySize);
  assert(copySize < dstSize);
  dst[copySize] = 0;
  return copySize;
}

const char * PerformAtCodePoints(const char * s, CodePoint c, CodePointAction actionCodePoint, CodePointAction actionOtherCodePoint, void * contextPointer, int contextInt1, int contextInt2, CodePoint stoppingCodePoint, bool goingRight, const char * initialPosition, const char * stoppingPosition) {
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
      while (*i != stoppingCodePoint && *i != 0 && i != stoppingPosition) {
        if (*i == c) {
          actionCodePoint(i - s, contextPointer, contextInt1, contextInt2);
        } else {
          // FIXME we are stopping at every char, not every code point -> it does not make any bug for now
          actionOtherCodePoint(i - s, contextPointer, contextInt1, contextInt2);
        }
        i++;
      }
      return i;
    }
    const char * i = initialPosition - 1;
    while (i >= s && *i != stoppingCodePoint && i != stoppingPosition) {
      if (*i == c) {
        actionCodePoint(i - s, contextPointer, contextInt1, contextInt2);
      } else {
        actionOtherCodePoint(i - s, contextPointer, contextInt1, contextInt2);
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
    while (codePoint != stoppingCodePoint && codePoint != UCodePointNull && codePointPointer != stoppingPosition) {
      if (codePoint == c) {
        actionCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
      } else {
        actionOtherCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
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
  while (codePointPointer >= s && codePoint != stoppingCodePoint && codePointPointer != stoppingPosition) {
    if (codePoint == c) {
      actionCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
    } else {
      actionOtherCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
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

int RemovePreviousGlyph(const char * text, char * location, CodePoint * c) {
  if (location <= text) {
    assert(location == text);
    return 0;
  }

  // Find the previous glyph
  UTF8Decoder decoder(text, location);
  const char * previousGlyphPos = decoder.previousGlyphPosition();
  if (c != nullptr) {
    *c = decoder.nextCodePoint();
  }

  // Shift the buffer
  int shiftedSize = location - previousGlyphPos;
  char * iterator = const_cast<char *>(previousGlyphPos);
  assert(iterator >= text);
  do {
    *iterator = *(iterator + shiftedSize);
    iterator++;
  } while (*(iterator - 1) != 0); // Stop shifting after writing a null terminating char.

  return shiftedSize;
}

const char * CodePointAtGlyphOffset(const char * buffer, int position) {
  assert(buffer != nullptr);
  if (position < 0) {
    return buffer;
  }

  UTF8Decoder decoder(buffer);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  int glyphIndex = 0;
  while (codePoint != UCodePointNull) {
    if (glyphIndex == position) {
      assert(!codePoint.isCombining());
      return codePointPointer;
    }
    if (!codePoint.isCombining()) {
      glyphIndex++;
    }
    codePointPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return codePointPointer;
}

size_t GlyphOffsetAtCodePoint(const char * buffer, const char * position) {
  assert(position >= buffer);

  UTF8Decoder decoder(buffer);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.nextCodePoint();
  size_t glyphIndex = 0;
  while (codePoint != UCodePointNull) {
    if (codePointPointer == position) {
      assert(!codePoint.isCombining());
      return glyphIndex;
    }
    if (!codePoint.isCombining()) {
      glyphIndex++;
    }
    codePointPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return glyphIndex;
}

size_t StringGlyphLength(const char * s, int maxSize) {
  if (maxSize == 0) {
    return 0;
  }
  UTF8Decoder decoder(s);
  CodePoint codePoint = 0;
  size_t glyphIndex = 0;
  while (maxSize < 0 || ((decoder.stringPosition() - s) < maxSize)) {
    codePoint = decoder.nextCodePoint();
    if (codePoint == UCodePointNull) {
      break;
    }
    if (!codePoint.isCombining()) {
      glyphIndex++;
    }
  }
  return glyphIndex;
}

}
