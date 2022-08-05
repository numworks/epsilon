#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

static inline int leading_ones(uint8_t value) {
  for (int i=0; i<8; i++) {
    if (!(value & 0x80)) {
      assert(i <= 4);
      return i;
    }
    value = value << 1;
  }
  assert(false);
  return 0;
}

static inline uint8_t last_k_bits(uint8_t value, uint8_t bits) {
  return (value & ((1<<bits)-1));
}

CodePoint UTF8Decoder::nextCodePoint() {
  assert(m_stringPosition == m_string || *(m_stringPosition - 1) != 0);
  int leadingOnes = leading_ones(*m_stringPosition);
  uint32_t result = last_k_bits(*m_stringPosition++, 8-leadingOnes-1);
  for (int i = 0; i < leadingOnes - 1; i++) {
    result <<= 6;
    char nextChunk = *m_stringPosition++;
    if (!nextChunk && 0x80) {
      /* The code point is not properly written. This might be due to a code
       * point being translated into chars in a too small buffer. */
      return UCodePointNull;
    }
    result += (nextChunk & 0x3F);
  }
  return CodePoint(result);
}

CodePoint UTF8Decoder::previousCodePoint() {
  assert(m_stringPosition > m_string);
  m_stringPosition--;
  int leadingOnes = leading_ones(*m_stringPosition);
  if (leadingOnes == 0) {
    // The current code point is one char long
    return *m_stringPosition;
  }
  // The current code point spans over multiple chars
  assert(leadingOnes == 1);
  uint32_t result = 0;
  int i = 0;
  while (leadingOnes == 1) {
    assert(m_stringPosition > m_string);
    result += (*m_stringPosition & 0x3F) << (6 * i);
    i++;
    m_stringPosition--;
    leadingOnes = leading_ones(*m_stringPosition);
  }

  assert(i <= 3);
  assert(leadingOnes > 1 && leadingOnes <= 4);
  assert(m_stringPosition >= m_string);

  result+= last_k_bits(*m_stringPosition, 8-leadingOnes-1) << (6*i);
  return CodePoint(result);
}

const char * UTF8Decoder::nextGlyphPosition() {
  assert(*m_stringPosition != 0 && (m_stringPosition == m_string || *(m_stringPosition - 1) != 0));
  CodePoint followingCodePoint = nextCodePoint();
  const char * resultGlyphPosition = m_stringPosition;
  followingCodePoint = nextCodePoint();
  while (followingCodePoint != UCodePointNull && followingCodePoint.isCombining()) {
    resultGlyphPosition = m_stringPosition;
    followingCodePoint = nextCodePoint();
  }
  m_stringPosition = resultGlyphPosition;
  return resultGlyphPosition;
}

const char * UTF8Decoder::previousGlyphPosition() {
  assert(m_stringPosition > m_string);
  CodePoint previousCP = previousCodePoint();
  const char * resultGlyphPosition = m_stringPosition;
  while (m_stringPosition > m_string && previousCP.isCombining()) {
    previousCP = previousCodePoint();
    resultGlyphPosition = m_stringPosition;
  }
  return resultGlyphPosition;
}

void UTF8Decoder::setPosition(const char * position) {
  assert(position >= m_string && position <= m_string + strlen(m_string));
  assert(!IsInTheMiddleOfACodePoint(*position));
  m_stringPosition = position;
}

size_t UTF8Decoder::CodePointToChars(CodePoint c, char * buffer, size_t bufferLength) {
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
