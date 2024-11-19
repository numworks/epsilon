#include "function.h"

#include <assert.h>
#include <float.h>
#include <ion/display.h>
#include <omg/utf8_decoder.h>
#include <poincare/old/serialization_helper.h>
#include <string.h>

#include <algorithm>
#include <cmath>

#include "interactive_curve_view_range.h"
#include "poincare_helpers.h"

using namespace Poincare;

namespace Shared {

size_t Function::WithArgument(CodePoint argument, char* buffer,
                              size_t bufferSize) {
  size_t length = SerializationHelper::CodePoint(buffer, bufferSize, '(');
  assert(UTF8Decoder::CharSizeOfCodePoint(argument) <= 2);
  length += SerializationHelper::CodePoint(buffer + length, bufferSize - length,
                                           argument);
  length +=
      SerializationHelper::CodePoint(buffer + length, bufferSize - length, ')');
  return length;
}

size_t Function::NameWithArgument(Ion::Storage::Record record,
                                  CodePoint argument, char* buffer,
                                  size_t bufferSize) {
  size_t length = record.nameWithoutExtension(buffer, bufferSize);
  return length + WithArgument(argument, buffer + length, bufferSize - length);
}

bool Function::isActive() const { return recordData()->isActive(); }

KDColor Function::color(int derivationOrder) const {
  return recordData()->color(derivationOrder);
}

void Function::setColor(KDColor color, int derivationOrder) {
  recordData()->setColor(color, derivationOrder);
}

void Function::setActive(bool active) {
  recordData()->setActive(active);
  if (!active) {
    didBecomeInactive();
  }
}

size_t Function::printAbscissaValue(double cursorT, double cursorX,
                                    char* buffer, size_t bufferSize,
                                    int precision) {
  return PoincareHelpers::ConvertFloatToText<double>(cursorT, buffer,
                                                     bufferSize, precision);
}

size_t Function::printFunctionValue(double cursorT, double cursorX,
                                    double cursorY, char* buffer,
                                    size_t bufferSize, int precision,
                                    Poincare::Context* context) {
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer,
                                                     bufferSize, precision);
}

size_t Function::name(char* buffer, size_t bufferSize) const {
  return nameWithoutExtension(buffer, bufferSize);
}

size_t Function::withArgument(char* buffer, size_t bufferSize) const {
  return Function::WithArgument(symbol(), buffer, bufferSize);
}

size_t Function::nameWithArgument(char* buffer, size_t bufferSize,
                                  int derivationOrder) {
  assert(derivationOrder == 0);
  return Function::NameWithArgument(*this, symbol(), buffer, bufferSize);
}

Function::RecordDataBuffer* Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer*>(const_cast<void*>(d.buffer));
}

}  // namespace Shared
