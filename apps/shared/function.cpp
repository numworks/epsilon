#include "function.h"
#include "interactive_curve_view_range.h"
#include "range_1D.h"
#include "poincare_helpers.h"
#include "poincare/src/parsing/parser.h"
#include <poincare/zoom.h>
#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

Function::NameNotCompliantError Function::BaseNameCompliant(const char * baseName) {
  assert(baseName[0] != 0);

  UTF8Decoder decoder(baseName);
  CodePoint c = decoder.nextCodePoint();
  if (c.isDecimalDigit()) {
    return NameNotCompliantError::NameCannotStartWithNumber;
  }

  while (c != UCodePointNull) {
    // FIXME '_' should be accepted but not as first character
    // TODO Factor this piece of code with similar one in the Parser
    if (!(c.isDecimalDigit() || c.isLatinLetter()) || c == '_') {
      return NameNotCompliantError::CharacterNotAllowed;
    }
    c = decoder.nextCodePoint();
  }

  if (Parser::IsReservedName(baseName, strlen(baseName))) {
    return NameNotCompliantError::ReservedName;
  }
  return NameNotCompliantError::None;
}

bool Function::isActive() const {
  return recordData()->isActive();
}

KDColor Function::color() const {
  return recordData()->color();
}

void Function::setActive(bool active) {
  recordData()->setActive(active);
}

int Function::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) {
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

int Function::name(char * buffer, size_t bufferSize) {
  return SymbolAbstract::TruncateExtension(buffer, fullName(), bufferSize);
}

int Function::nameWithArgument(char * buffer, size_t bufferSize) {
  int funcNameSize = name(buffer, bufferSize);
  assert(funcNameSize > 0);
  size_t result = funcNameSize;
  assert(result <= bufferSize);
  buffer[result++] = '(';
  assert(result <= bufferSize);
  assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
  result += UTF8Decoder::CodePointToChars(symbol(), buffer+result, bufferSize-result);
  assert(result <= bufferSize);
  result += strlcpy(buffer+result, ")", bufferSize-result);
  return result;
}

Function::RecordDataBuffer * Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

void Function::protectedRangeForDisplay(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context, bool boundByMagnitude) const {
  Zoom::ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
  /* When evaluating sin(x)/x close to zero using the standard sine function,
   * one can detect small variations, while the cardinal sine is supposed to be
   * locally monotonous. To smooth our such variations, we round the result of
   * the evaluations. As we are not interested in precise results but only in
   * ordering, this approximation is sufficient. */
    constexpr float precision = 1e-5;
    return precision * std::round(static_cast<const Function *>(auxiliary)->evaluateXYAtParameter(x, context).x2() / precision);
  };
  bool fullyComputed = Zoom::InterestingRangesForDisplay(evaluation, xMin, xMax, yMin, yMax, tMin(), tMax(), context, this);

  evaluation = [](float x, Context * context, const void * auxiliary) {
    return static_cast<const Function *>(auxiliary)->evaluateXYAtParameter(x, context).x2();
  };

  if (fullyComputed) {
    Zoom::RefinedYRangeForDisplay(evaluation, *xMin, *xMax, yMin, yMax, context, this, boundByMagnitude);
  } else {
    Zoom::RangeWithRatioForDisplay(evaluation, InteractiveCurveViewRange::NormalYXRatio(), xMin, xMax, yMin, yMax, context, this);
  }
}
}
