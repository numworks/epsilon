#pragma once

#include <poincare/symbol_store_interface.h>
#include <poincare/user_expression.h>

namespace Poincare {

namespace StoreHelper {

UserExpression Value(UserExpression e);
UserExpression Symbol(UserExpression e);

}  // namespace StoreHelper

}  // namespace Poincare
