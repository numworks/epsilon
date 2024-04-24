#include <assert.h>
#include <omg/utf8_decoder.h>
#include <string.h>

size_t ForwardUnicodeDecoder::nextGlyphPosition() {
  CodePoint followingCodePoint = nextCodePoint();
  size_t resultGlyphPosition = m_position;
  followingCodePoint = nextCodePoint();
  while (followingCodePoint != UCodePointNull &&
         followingCodePoint.isCombining()) {
    resultGlyphPosition = m_position;
    followingCodePoint = nextCodePoint();
  }
  m_position = resultGlyphPosition;
  return resultGlyphPosition;
}

size_t UnicodeDecoder::previousGlyphPosition() {
  assert(m_position > 0);
  CodePoint previousCP = previousCodePoint();
  size_t resultGlyphPosition = m_position;
  while (m_position > 0 && previousCP.isCombining()) {
    previousCP = previousCodePoint();
    resultGlyphPosition = m_position;
  }
  return resultGlyphPosition;
}

size_t ForwardUnicodeDecoder::printInBuffer(char* buffer, size_t bufferSize,
                                            size_t printLength) {
  assert(m_end == k_noSize || printLength == k_noSize ||
         m_position + printLength <= m_end);
  size_t result = 0;
  while (result < printLength) {
    CodePoint c = nextCodePoint();
    if (c == UCodePointNull) {
      break;
    }
    if (result + UTF8Decoder::CharSizeOfCodePoint(c) >= bufferSize) {
      buffer[0] = 0;
      return 0;
    }
    result +=
        UTF8Decoder::CodePointToChars(c, buffer + result, bufferSize - result);
  }
  buffer[result] = 0;
  return result;
}

static inline int leading_ones(uint8_t value) {
  int i;
  for (i = 0; (value & 0x80) != 0; i++) {
    value = value << 1;
    assert(i <= 8);
  }
  return i;
}

// An UTF-8 char is invalid if it has more than 4 leading ones.
static bool isValidChar(int value_leading_ones) {
  return value_leading_ones <= 4;
}

static inline uint8_t last_k_bits(uint8_t value, uint8_t bits) {
  return (value & ((1 << bits) - 1));
}

CodePoint UTF8Decoder::nextCodePoint() {
  assert((stringPosition() == m_string || *(stringPosition() - 1) != 0) &&
         (stringEnd() == nullptr || m_position <= m_end));
  bool returnCodePointNull = false;
  if (stringEnd() != nullptr && stringPosition() == stringEnd()) {
    returnCodePointNull = true;
  }

  int leadingOnes = leading_ones(*stringPosition());

  if (!isValidChar(leadingOnes)) {
    nextByte();
    return UCodePointReplacement;
  }

  uint32_t result = last_k_bits(nextByte(), 8 - leadingOnes - 1);
  for (int i = 0; i < leadingOnes - 1; i++) {
    result <<= 6;
    char nextChunk = nextByte();
    if (!nextChunk && 0x80) {
      /* The code point is not properly written. This might be due to a code
       * point being translated into chars in a too small buffer. */
      return UCodePointNull;
    }
    result += (nextChunk & 0x3F);
  }
  return returnCodePointNull ? UCodePointNull : CodePoint(result);
}

CodePoint UTF8Decoder::previousCodePoint() {
  assert(stringPosition() > m_string);
  previousByte();
  int leadingOnes = leading_ones(*stringPosition());
  if (leadingOnes == 0) {
    // The current code point is one char long
    return *stringPosition();
  }
  // The current code point spans over multiple chars
  assert(leadingOnes == 1);
  uint32_t result = 0;
  int i = 0;
  while (leadingOnes == 1) {
    assert(stringPosition() > m_string);
    result += (*stringPosition() & 0x3F) << (6 * i);
    i++;
    previousByte();
    leadingOnes = leading_ones(*stringPosition());

    if (!isValidChar(leadingOnes)) {
      return UCodePointReplacement;
    }
  }

  assert(i <= 3);
  assert(leadingOnes > 1 && leadingOnes <= 4);
  assert(stringPosition() >= m_string);

  result += last_k_bits(*stringPosition(), 8 - leadingOnes - 1) << (6 * i);
  return CodePoint(result);
}

void UTF8Decoder::setPosition(const char* position) {
  assert(position >= string() && position <= string() + strlen(string()));
  assert(!IsInTheMiddleOfACodePoint(*position));
  m_position = position - m_string;
}

size_t UTF8Decoder::CodePointToChars(CodePoint c, char* buffer,
                                     size_t bufferLength) {
  size_t i = 0;
  size_t charCount = CharSizeOfCodePoint(c);
  assert(bufferLength >= charCount);
  if (charCount == 1) {
    buffer[i++] = c;
  } else if (charCount == 2) {
    buffer[i++] = 0b11000000 | (c >> 6);
    buffer[i++] = 0b10000000 | (c & 0b111111);
  } else if (charCount == 3) {
    buffer[i++] = 0b11100000 | (c >> 12);
    buffer[i++] = 0b10000000 | ((c >> 6) & 0b111111);
    buffer[i++] = 0b10000000 | (c & 0b111111);
  } else {
    assert(charCount == 4);
    buffer[i++] = 0b11110000 | (c >> 18);
    buffer[i++] = 0b10000000 | ((c >> 12) & 0b111111);
    buffer[i++] = 0b10000000 | ((c >> 6) & 0b111111);
    buffer[i++] = 0b10000000 | (c & 0b111111);
  }
  assert(i == charCount);
  return charCount;
}

bool UTF8Decoder::IsInTheMiddleOfACodePoint(uint8_t value) {
  return value >= 0b10000000 && value < 0b11000000;
}

bool UTF8Decoder::IsTheEndOfACodePoint(const char* end, const char* begin) {
  int numberOfBytes = 0;
  for (const uint8_t* ptr = reinterpret_cast<const uint8_t*>(end);
       ptr >= reinterpret_cast<const uint8_t*>(begin); ptr--) {
    int ones = leading_ones(*ptr);
    assert(isValidChar(ones));
    numberOfBytes++;
    assert(numberOfBytes <= 4);
    switch (ones) {
      case 1:
        break;
      case 0:
        return numberOfBytes == 1;
      default:
        return ones == numberOfBytes;
    }
  }

  return false;
}
