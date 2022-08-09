#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <kandinsky/font.h>
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
      if (*i == c.getChar()) {
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

const char * CodePointSearch(const char * s, CodePoint c, const char * stoppingPosition) {
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    const char * result = s;
    while (*result != 0 && *result != c.getChar() && (stoppingPosition == nullptr || result != stoppingPosition)) {
      result++;
    }
    return result;
  }
  UTF8Decoder decoder(s);
  const char * currentPointer = s;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  while (codePoint != UCodePointNull && codePoint != c && (stoppingPosition == nullptr || currentPointer < stoppingPosition)) {
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  return currentPointer;
}

bool HasCodePoint(const char * s, CodePoint c, const char * stoppingPosition) {
  assert(c != 0);
  const char * resultPosition = CodePointSearch(s, c, stoppingPosition);
  return *resultPosition != 0 && (stoppingPosition == nullptr || resultPosition < stoppingPosition);
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
    while ((goingLeft ?codePointPointer > s : *codePointPointer != 0) && *codePointPointer == c.getChar()) {
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

int CompareNonNullTerminatedStringWithNullTerminated(const char * nonNullTerminatedString, size_t nonNullTerminatedStringLength, const char * nullTerminatedString) {
  int diff = strncmp(nonNullTerminatedString, nullTerminatedString, nonNullTerminatedStringLength);
  return (diff != 0) ? diff : strcmp("", nullTerminatedString + nonNullTerminatedStringLength);
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
  // +1 for null terminating char
  constexpr int patternMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char pattern[patternMaxSize];
  int codePointCharSize = UTF8Decoder::CodePointToChars(c, pattern, patternMaxSize - 1);
  assert(codePointCharSize < patternMaxSize);
  pattern[codePointCharSize] = '\0';
  TextPair pair(pattern, "");
  TryAndReplacePatternsInStringByPatterns(buffer, strlen(buffer), &pair, 1, true, pointerToUpdate, stoppingPosition);
}

bool SlideStringByNumberOfChar(char * text, int slidingSize, size_t textMaxLength) {
  const size_t textLen = strlen(text);
  if (textLen + slidingSize > textMaxLength || textLen + slidingSize < 0) {
    return false;
  }
  if (slidingSize > 0) {
    memmove(text+slidingSize, text, textLen + 1);
  } else if (slidingSize < 0) {
    memmove(text, text-slidingSize, textLen + 1);
  }
  // In case slidingSize = 0, there is nothing to do
  return true;
}

/* Replaces the first chars of a string by other ones. If the sizes are different
 * the rest of the string will be moved right after the replacement chars.
 * If successful returns true.*/
static bool replaceFirstCharsByPattern(char * text, size_t lengthOfPatternToRemove, const char * replacementPattern, size_t textMaxLength) {
  size_t lengthOfReplacementPattern = strlen(replacementPattern);
  if (lengthOfPatternToRemove <= strlen(text) && SlideStringByNumberOfChar(text, lengthOfReplacementPattern-lengthOfPatternToRemove, textMaxLength)) {
    for (size_t i = 0; i < lengthOfReplacementPattern; i++) {
      text[i] = replacementPattern[i];
    }
    return true;
  }
  return false;
}

void TryAndReplacePatternsInStringByPatterns(char * text, int textMaxLength, const TextPair * textPairs, int numberOfPairs, bool firstToSecond, const char * * pointerToUpdate, const char * stoppingPosition) {
  size_t i = 0;
  size_t iPrev = 0;
  size_t textLength = strlen(text);
  size_t lengthOfParenthesisExtension = strlen("(\x11)");
  while(i < textLength) {
    iPrev = i;
    bool didReplace = false;
    for (int j = 0; j < numberOfPairs; j++) {
      TextPair p = textPairs[j];
      size_t firstStringLength = strlen(p.firstString());
      size_t secondStringLength = strlen(p.secondString());
      /* Instead of storing TextPair("√(\x11)", "sqrt(\x11)") for the keyboard
       * events and TextPair("√", "sqrt") for the copy paste, we store just the
       * first and register it as "function". Therefore we can decide to remove
       * the (\x11) part or not depending on the application. This process is
       * repeated for all 4 function keys usable in python (√, e, ln, log)*/
      if (p.removeParenthesesExtension()) {
        firstStringLength -= lengthOfParenthesisExtension;
        secondStringLength -= lengthOfParenthesisExtension;
      }
      char firstString[TextPair::k_maxLength];
      char secondString[TextPair::k_maxLength];
      // Getting rid of the eventual (\x11) part
      strlcpy((char *)firstString, p.firstString(), firstStringLength+1);
      strlcpy((char *)secondString, p.secondString(), secondStringLength+1);

      char * matchedString = firstToSecond ? firstString : secondString;
      size_t matchedStringLength = strlen(matchedString);
      char * replacingString = firstToSecond ? secondString : firstString;
      size_t replacingStringLength = strlen(replacingString);
      assert(matchedStringLength > 0);

      if (strncmp(&text[i], matchedString, matchedStringLength) == 0 && p.shouldReplace(text, textMaxLength, i)) {
        didReplace = replaceFirstCharsByPattern(&text[i], matchedStringLength, replacingString, textMaxLength - i);
        if (didReplace) {
          int delta = replacingStringLength - matchedStringLength;
          textLength += delta;
          if (pointerToUpdate != nullptr && &text[i] < *pointerToUpdate) {
            // We still have to update the pointer as the modification cursor has not yet exceeded it.
            *pointerToUpdate = *pointerToUpdate + delta;
          }
          if (stoppingPosition != nullptr) {
            stoppingPosition = stoppingPosition + delta;
          }
          if (replacingStringLength != 0) {
            i += replacingStringLength - 1;
            /* When working with multiple TextPairs at the same time, it can be
             * usefull to go back by one char. That is the case for empty matrixes
             * Indeed, in the string ",,]",  ",," is replaced by ",\x11,".
             * The ",]" pattern right after would be missed if not for the -1.*/
          }
        }
      }
    }
    if (iPrev == i && !didReplace) {
      // In case no pattern matched with the text, we go to the next char.
      i++;
    }
    if ((stoppingPosition != nullptr) && (&text[i] >= stoppingPosition)) {
      break;
    }
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
      while (*i != stoppingCodePoint.getChar() && *i != 0 && i != stoppingPosition) {
        if (*i == c.getChar()) {
          if (actionCodePoint) {
            actionCodePoint(i - s, contextPointer, contextInt1, contextInt2);
          }
        } else {
          // FIXME we are stopping at every char, not every code point -> it does not make any bug for now
          if (actionOtherCodePoint) {
            actionOtherCodePoint(i - s, contextPointer, contextInt1, contextInt2);
          }
        }
        i++;
      }
      return i;
    }
    const char * i = initialPosition - 1;
    while (i >= s && *i != stoppingCodePoint.getChar() && i != stoppingPosition) {
      if (*i == c.getChar()) {
        if (actionCodePoint) {
          actionCodePoint(i - s, contextPointer, contextInt1, contextInt2);
        }
      } else {
        if (actionOtherCodePoint) {
          actionOtherCodePoint(i - s, contextPointer, contextInt1, contextInt2);
        }
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
        if (actionCodePoint) {
          actionCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
        }
      } else {
        if (actionOtherCodePoint) {
          actionOtherCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
        }
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
      if (actionCodePoint) {
        actionCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
      }
    } else {
      if (actionOtherCodePoint) {
        actionOtherCodePoint(codePointPointer - s, contextPointer, contextInt1, contextInt2);
      }
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

CodePoint PreviousCodePoint(const char * buffer, const char * location) {
  if (location == buffer) {
    return UCodePointNull;
  }
  UTF8Decoder decoder(buffer, location);
  return decoder.previousCodePoint();
}

CodePoint CodePointAtLocation(const char * location) {
  UTF8Decoder decoder(location);
  return decoder.nextCodePoint();
}

bool PreviousCodePointIs(const char * buffer, const char * location, CodePoint c) {
  assert(location > buffer);
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    return *(location -1) == c.getChar();
  }
  return PreviousCodePoint(buffer, location) == c;
}

bool CodePointIs(const char * location, CodePoint c) {
  if (UTF8Decoder::CharSizeOfCodePoint(c) == 1) {
    return *(location) == c.getChar();
  }
  return CodePointAtLocation(location) == c;
}

bool CodePointIsEndOfWord(CodePoint c) {
  return c == '\n' || c == ' ' || c == UCodePointNull;
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
    // Stop shifting after writing a null terminating char.
  } while (*(iterator - 1) != 0);

  return shiftedSize;
}

const char * CodePointAtGlyphOffset(const char * buffer, int position) {
  assert(buffer != nullptr);
  if (position < 0 || buffer[0] == '\0') {
    return buffer;
  }

  UTF8Decoder decoder(buffer);
  for (int i = 0; i < position; i++) {
    if (*decoder.nextGlyphPosition() == '\0') {
      break;
    }
  }
  return decoder.stringPosition();
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

const char * BeginningOfWord(const char * text, const char * word) {
  if (text == word) {
    return text;
  }
  UTF8Decoder decoder(text, word);
  const char * codePointPointer = decoder.stringPosition();
  CodePoint codePoint = decoder.previousCodePoint();
  while (!CodePointIsEndOfWord(codePoint)) {
    codePointPointer = decoder.stringPosition();
    if (codePointPointer == text) {
      break;
    }
    codePoint = decoder.previousCodePoint();
  }
  return codePointPointer;
}

const char * EndOfWord(const char * word) {
  UTF8Decoder decoder(word);
  CodePoint codePoint = decoder.nextCodePoint();
  const char * result = word;
  while (!CodePointIsEndOfWord(codePoint)) {
    result = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return result;
}

void CountGlyphsInLine(const char * text, int * before, int * after, const char * beforeLocation, const char *afterLocation) {
  UTF8Helper::CodePointAction countGlyph = [](int, void * glyphCount, int, int) {
    int * castedCount = (int *) glyphCount;
    *castedCount = *castedCount + 1;
  };
  // Count glyphs before
  UTF8Helper::PerformAtCodePoints(text, UCodePointLineFeed, nullptr, countGlyph, before, 0, 0, UCodePointLineFeed, false, beforeLocation);
  if (afterLocation == nullptr) {
    afterLocation = beforeLocation;
  }
  // Count glyphs after
  UTF8Helper::PerformAtCodePoints(afterLocation, UCodePointLineFeed, nullptr, countGlyph, after, 0, 0, UCodePointLineFeed);
}

const char * SuffixCaseInsensitiveNoCombining(const char * a, const char * b) {
  UTF8Decoder aDecoder(a);
  UTF8Decoder bDecoder(b);
  CodePoint a0 = aDecoder.nextCodePoint(), b0 = bDecoder.nextCodePoint();
  while (a0 != UCodePointNull) {
    if (a0.isCombining()) {
      a0 = aDecoder.nextCodePoint();
    } else if (b0.isCombining()) {
      b0 = bDecoder.nextCodePoint();
    } else {
      if (a0.isLatinSmallLetter() && b0.isLatinCapitalLetter()) {
        a0 = a0.getChar() - 'a' + 'A';
      } else if (b0.isLatinSmallLetter() && a0.isLatinCapitalLetter()) {
        b0 = b0.getChar() - 'a' + 'A';
      }
      if (a0 != b0) {
        return nullptr;
      }
      a0 = aDecoder.nextCodePoint();
      b0 = bDecoder.nextCodePoint();
    }
  }
  while (b0.isCombining()) {
    b0 = bDecoder.nextCodePoint();
  }
  bDecoder.previousCodePoint();
  return bDecoder.stringPosition();
}

bool IsPrefixCaseInsensitiveNoCombining(const char * a, const char * b) {
  return static_cast<bool>(SuffixCaseInsensitiveNoCombining(a, b));
}

}
