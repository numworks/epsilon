#pragma once

#include <poincare/symbol_store_interface.h>
#include <poincare/user_expression.h>

namespace Poincare {

namespace StoreHelper {

const UserExpression Value(const UserExpression& e);
const UserExpression Symbol(const UserExpression& e);

}  // namespace StoreHelper

}  // namespace Poincare
