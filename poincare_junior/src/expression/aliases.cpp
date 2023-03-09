#include <ion/unicode/utf8_helper.h>
#include "aliases.h"

namespace PoincareJ {

static int CompareDecoders(UnicodeDecoder * a, UnicodeDecoder * b) {
  while (CodePoint c = a->nextCodePoint()) {
    CodePoint d = b->nextCodePoint();
    if (c != d) {
      return c - d;
    }
  }
  return b->nextCodePoint();
}

static int CompareDecoderWithNullTerminatedString(UnicodeDecoder * decoder, const char * string) {
  // TODO this UnicodeDecoder API is aweful
  size_t position = decoder->position();
  UTF8Decoder stringDecoder(string);
  int result = CompareDecoders(decoder, &stringDecoder);
  decoder->unsafeSetPosition(position);
  return result;
}

int Aliases::maxDifferenceWith(UnicodeDecoder * decoder) const {
  if (!hasMultipleAliases()) {
    return CompareDecoderWithNullTerminatedString(decoder, m_formattedAliases);
  }
  int maxValueOfComparison = 0;
  for (const char* aliasInList : *this) {
    int tempValueOfComparison =
        CompareDecoderWithNullTerminatedString(decoder, aliasInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison ||
        maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
  }
  return maxValueOfComparison;
}

const char* Aliases::nextAlias(
    const char* currentPositionInAliases) const {
  if (!hasMultipleAliases()) {
    return nullptr;
  }
  assert(strlen(currentPositionInAliases) != 0);
  const char* beginningOfNextAlias =
      currentPositionInAliases + strlen(currentPositionInAliases) + 1;
  if (beginningOfNextAlias[0] == 0) {
    return nullptr;  // End of list
  }
  return beginningOfNextAlias;
}

}  // namespace PoincareJ
