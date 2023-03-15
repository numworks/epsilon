#include "function.h"

#include <assert.h>
#include <float.h>
#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/zoom.h>
#include <string.h>

#include <algorithm>
#include <cmath>

#include "interactive_curve_view_range.h"
#include "poincare/src/parsing/parser.h"
#include "poincare_helpers.h"

using namespace Poincare;

namespace Shared {

bool Function::isActive() const { return recordData()->isActive(); }

KDColor Function::color() const { return recordData()->color(); }

void Function::setColor(KDColor color) { recordData()->setColor(color); }

void Function::setActive(bool active) {
  recordData()->setActive(active);
  if (!active) {
    didBecomeInactive();
  }
}

int Function::printValue(double cursorT, double cursorX, double cursorY,
                         char *buffer, int bufferSize, int precision,
                         Poincare::Context *context, bool symbolValue) {
  return PoincareHelpers::ConvertFloatToText<double>(
      symbolValue ? cursorT : cursorY, buffer, bufferSize, precision);
}

int Function::name(char *buffer, size_t bufferSize) const {
  return SymbolAbstract::TruncateExtension(buffer, fullName(), bufferSize);
}

int Function::nameWithArgument(char *buffer, size_t bufferSize) {
  int funcNameSize = name(buffer, bufferSize);
  assert(funcNameSize > 0);
  size_t result = funcNameSize;
  assert(result <= bufferSize);
  buffer[result++] = '(';
  assert(result <= bufferSize);
  assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
  result += UTF8Decoder::CodePointToChars(symbol(), buffer + result,
                                          bufferSize - result);
  assert(result <= bufferSize);
  result += strlcpy(buffer + result, ")", bufferSize - result);
  return result;
}

Function::RecordDataBuffer *Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

}  // namespace Shared
