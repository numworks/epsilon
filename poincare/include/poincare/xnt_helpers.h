#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

bool FindXNTSymbol(UnicodeDecoder & decoder, bool * defaultXNTHasChanged, CodePoint * defaultXNTCodePoint);

}

#endif
