#pragma once

#include <poincare/symbol_store_interface.h>
#include <poincare/user_expression.h>

namespace Poincare {

namespace StoreHelper {

UserExpression Value(UserExpression e);
UserExpression Symbol(UserExpression e);

/* Replaces the value to store in the "e" store expression by valueToStore.
 * Returns the newly formed store expression. */
UserExpression SetExpressionToStore(UserExpression e,
                                    UserExpression valueToStore);

}  // namespace StoreHelper

}  // namespace Poincare
