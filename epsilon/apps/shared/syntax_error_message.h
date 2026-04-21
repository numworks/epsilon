#pragma once

#include <apps/i18n.h>
#include <poincare/syntax_error.h>

namespace Shared {

I18n::Message MessageForSyntaxError(Poincare::SyntaxError error);

}  // namespace Shared
