
#include "syntax_error_message.h"

#include <assert.h>

namespace Shared {

I18n::Message MessageForSyntaxError(Poincare::SyntaxError error) {
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
