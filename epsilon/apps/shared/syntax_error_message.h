#pragma once

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/syntax_error.h>

namespace Shared {

inline I18n::Message MessageForSyntaxError(Poincare::SyntaxError error) {
  assert(error != Poincare::SyntaxError::None);
  return I18n::Message::SyntaxError;
}

}  // namespace Shared
