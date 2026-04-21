#pragma once

#include <apps/i18n.h>
#include <assert.h>
#include <poincare/syntax_error.h>

namespace Shared {

inline I18n::Message MessageForSyntaxError(Poincare::SyntaxError error) {
  switch (error) {
    case Poincare::SyntaxError::DataNotSuitable:
      return I18n::Message::DataNotSuitable;
    case Poincare::SyntaxError::None:
      assert(false);
    default:
      return I18n::Message::SyntaxError;
  }
}

}  // namespace Shared
