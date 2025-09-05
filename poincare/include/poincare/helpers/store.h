#pragma once

#include <poincare/user_expression.h>
#include <poincare/variable_store.h>

namespace Poincare {

namespace StoreHelper {

const UserExpression Value(const UserExpression& e);
const UserExpression Symbol(const UserExpression& e);

}  // namespace StoreHelper

}  // namespace Poincare
