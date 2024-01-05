#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <ion/unicode/utf8_decoder.h>
#include <poincare_layouts.h>

namespace Poincare {

namespace XNTHelpers {

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize);

bool FindXNTSymbol2D(Layout layout, char* buffer, size_t bufferSize);

}  // namespace XNTHelpers

}  // namespace Poincare

#endif
