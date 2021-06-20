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
  if (!active) {
    didBecomeInactive();
  }
}

void Function::setColor(KDColor color) {
  recordData()->setColor(color);
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

void Function::protectedFullRangeForDisplay(float tMin, float tMax, float tStep, float * min, float * max, Poincare::Context * context, bool xRange) const {
  Poincare::Zoom::ValueAtAbscissa evaluation;
  if (xRange) {
    evaluation = [](float x, Poincare::Context * context, const void * auxiliary) {
      return static_cast<const Function *>(auxiliary)->evaluateXYAtParameter(x, context).x1();
    };
  } else {
    evaluation = [](float x, Poincare::Context * context, const void * auxiliary) {
      return static_cast<const Function *>(auxiliary)->evaluateXYAtParameter(x, context).x2();
    };
  }

  Poincare::Zoom::FullRange(evaluation, tMin, tMax, tStep, min, max, context, this);
}

}
